#include "TimerQueueff.h"
#include "Timestampff.h"
#include "Channelff.h"
#include "EventLoopff.h"
#include "Timerff.h"
#include "TimerIdff.h"

#include <assert.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>

using namespace firey;

int createTimerfd(){
	int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
	if(timerfd<0) abort();
	return timerfd;
}

struct timespec howMuchTimeFromNow(Timestampff when){
	int64_t microSecond=when.usSinceEpoch()-Timestampff::now().usSinceEpoch();
	if(microSecond<100){
		microSecond=100;
	}

	struct timespec tspc;
	memset(&tspc,0,sizeof tspc);
	tspc.tv_sec=static_cast<time_t>(microSecond/Timestampff::kusPerSecond);
	tspc.tv_nsec=static_cast<time_t>(microSecond%Timestampff::kusPerSecond*1000);
	return tspc;
}

void resetTimerfd(int timerfd,Timestampff when){
	struct itimerspec newValue;
	struct itimerspec oldValue;
	memset(&newValue,0,sizeof newValue);
	memset(&oldValue,0,sizeof oldValue);

	newValue.it_value=howMuchTimeFromNow(when);

	int ret=::timerfd_settime(timerfd,0,&newValue,&oldValue);
	if(ret){
		fprintf(stderr,"resetTimerfd %s,%s,%d,%s",__FILE__,__func__,__LINE__,strerror(ret));
	}
}

void readTimerfd(int timerfd,Timestampff now){
	uint64_t howmany;
	ssize_t n=::read(timerfd,&howmany,sizeof howmany);
	if(n!=sizeof howmany){
		fprintf(stderr,"TimerQueue::handleRead() reads %d bytes instead of 8",static_cast<int>(n));
	}
}

TimerQueueff::TimerQueueff(EventLoopff* loop)
	:ownerLoop_(loop),
	timerFd_(createTimerfd()),
	timerChannel_(new Channelff(ownerLoop_,timerFd_)),
	callingExpiredTimers_(false)
{
	timerChannel_->setReadCallBack(
			std::bind(&TimerQueueff::handleTimerfdRead,this));
	timerChannel_->enableReading();
}

TimerQueueff::~TimerQueueff(){
	timerChannel_->disableAll();
	timerChannel_->remove();

	::close(timerFd_);

	for(auto& timer:timers_){
		delete timer.second;
	}
}


//当timerfd可读时，表明有定时器到期，读取timerfd，并获取到期的定时器，执行之
//如果定时器时重复的，并且没有被取消，那么restart它
void TimerQueueff::handleTimerfdRead(){
	ownerLoop_->assertInLoopThread();
	Timestampff now(Timestampff::now());

	readTimerfd(timerFd_,now);
	
	std::vector<TimerNode> expiredTimer=getExpiredTimer(now);
	callingExpiredTimers_=true;

	cancelingTimers_.clear();

	for(auto& timer:expiredTimer){
		timer.second->run();
	}
	callingExpiredTimers_=false;
	//reset some repeat timer
	reset(expiredTimer,now);
}

void TimerQueueff::reset(std::vector<TimerQueueff::TimerNode> expired,Timestampff now){
	ownerLoop_->assertInLoopThread();
	
	for(const auto& it:expired){
		ActiveTimer timer(it.second,it.second->sequence());

		if(it.second->isRepeat()&&cancelingTimers_.find(timer)==cancelingTimers_.end()){
			it.second->restart(now);
			insertTimer(it.second);	
		}
		else delete it.second;
	}

	Timestampff nextExpire;
	if(!timers_.empty()){
		nextExpire=timers_.begin()->first;
	}
	
	if(nextExpire.isValid()){
		resetTimerfd(timerFd_,nextExpire);
	}
}

std::vector<TimerQueueff::TimerNode> TimerQueueff::getExpiredTimer(Timestampff now){
	assert(timers_.size()==activeTimers_.size());

	std::vector<TimerNode> expired;

	TimerNode sentry(now,reinterpret_cast<Timerff*>(UINTPTR_MAX));

	//find the first timer bigger than now
	TimerTree::iterator it=timers_.lower_bound(sentry);
	assert(it==timers_.end()||it->first>now);
	std::copy(timers_.begin(),it,back_inserter(expired));
	timers_.erase(timers_.begin(),it);

	for(auto& timer:expired){
		ActiveTimer tempTimer(timer.second,timer.second->sequence());
		activeTimers_.erase(tempTimer);
	}

	assert(timers_.size()==activeTimers_.size());
	return expired;
}

//插入定时器
TimerIdff TimerQueueff::addTimer(timerCallback cb,Timestampff when,double interval){
	Timerff* timer=new Timerff(std::move(cb),when,interval);
	ownerLoop_->runInLoop(
			std::bind(&TimerQueueff::addTimerInLoop,this,timer));
	return TimerIdff(timer,timer->sequence());
}

void TimerQueueff::addTimerInLoop(Timerff* timer){
	ownerLoop_->assertInLoopThread();

	bool earliestChanged = insertTimer(timer);

	if(earliestChanged){
		resetTimerfd(timerFd_,timer->expireTime());
	}
}

bool TimerQueueff::insertTimer(Timerff* timer){
	ownerLoop_->assertInLoopThread();
	
	assert(timers_.size()==activeTimers_.size());

	bool earliestChanged=false;

	Timestampff when= timer->expireTime();

	TimerTree::iterator it=timers_.begin();
	if(it==timers_.end()||when<it->first){
		earliestChanged=true;
	}

	timers_.insert({when,timer});
	activeTimers_.insert({timer,timer->sequence()});

	assert(timers_.size()==activeTimers_.size());
	return earliestChanged;
}

//cancel timer 
void TimerQueueff::cancel(TimerIdff timerId){
	ownerLoop_->runInLoop(
			std::bind(&TimerQueueff::cancelInLoop,this,timerId));
}

void TimerQueueff::cancelInLoop(TimerIdff timerId){
	ownerLoop_->assertInLoopThread();

	assert(timers_.size()==activeTimers_.size());
	ActiveTimer timer(timerId.timer(),timerId.sequence());
	activeTimerSet::iterator it=activeTimers_.find(timer);
	if(it!=activeTimers_.end()){
		timers_.erase(TimerNode(it->first->expireTime(),it->first));
		delete it->first;
		activeTimers_.erase(it);
	}
	//当正在调用过期的定时器时，有可能要取消的定时器不在两个定时器集合中(timers_和activeTimers_)
	//此时无法从两个集合中直接删除，需要将其推入待取消的定时器队列中，待过期的定时器任务执行完毕之后
	//再重置到期的定时器，此时如果该定时在待取消的定时器队列中，则不重置它（或者非重复定时器也不重置）
	else if(callingExpiredTimers_){
		cancelingTimers_.insert(timer);
	}

	assert(timers_.size()==activeTimers_.size());
}	
