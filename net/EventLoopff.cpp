#include "EventLoopff.h"
#include "Pollerff.h"
#include "Channelff.h"
#include "MutexGuardff.h"
#include "TimerQueueff.h"
#include "Timestampff.h"
#include "Loggingff.h"

#include <assert.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <stdio.h>

using namespace firey;

__thread EventLoopff* t_loopInThisThread=nullptr;

void EventLoopff::abortNotInLoopThread(){
	LOG_FATAL<<"EventLoop::abortNotInLoopThread() - EventLoop: "<<this
		<<" was created int threadId_ = "<<threadId_
		<<", current thread id = "<<CurrentThreadff::tid();
}

int createEventFd(){
	int evfd=::eventfd(0,EFD_CLOEXEC|EFD_NONBLOCK);
	if(evfd<0)
	{
		LOG_SYSFATAL<<"Failed in create eventfd";
	}
	return evfd;
}

struct ffPipeIgnore{
	ffPipeIgnore(){
		::signal(SIGPIPE,SIG_IGN);
	}
};

ffPipeIgnore pipeIgnoreObj;

EventLoopff::EventLoopff()
	:looping_(false),
	quit_(false),
	eventHandling_(false),
	threadId_(CurrentThreadff::tid()),
	poller_(new Pollerff(this)),
	callingPendingFunctors_(false),
	wakeupFd_(createEventFd()),
	wakeupChannel_(new Channelff(this,wakeupFd_)),
	timerQueue_(new TimerQueueff(this))	
{
	LOG_DEBUG<<"EventLoop created "<<this<<" in thread "<<threadId_;
	if(t_loopInThisThread)
	{
		LOG_FATAL<<"Another EventLoop "<<t_loopInThisThread
			<<" exists in this thread "<<threadId_;
	}
	t_loopInThisThread=this;

	wakeupChannel_->setReadCallback(
			std::bind(&EventLoopff::handleWakeupRead,this));
	wakeupChannel_->enableReading();
}

EventLoopff::~EventLoopff(){
	LOG_DEBUG<<"EventLoop "<<this<<" of thread "<<threadId_
		<<" destructs in thread "<<CurrentThreadff::tid();
	assert(!looping_);
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
	t_loopInThisThread=nullptr;
}

void EventLoopff::loop(){
	assert(!looping_);
	assertInLoopThread();
	looping_=true;

	LOG_TRACE<<"EventLoop "<<this<<" start looping";
	while(!quit_)
	{
		activeChannels_.clear();
		pollReturnTime_=poller_->poll(kPollWaitTime,&activeChannels_);
		
		eventHandling_=true;
		for(auto ach:activeChannels_){
			ach->handleEvent(pollReturnTime_);
		}
		eventHandling_=false;
		
		doPendingFunctors();
	}
	LOG_TRACE<<"EventLoop "<<this<<" stop looping";
	looping_=false;
}

void EventLoopff::quit(){
	assert(!quit_);
	quit_=true;
	if(!isInLoopThread())
	{
		wakeup();
	}
}

//channel的管理
void EventLoopff::updateChannel(Channelff* channel){
	assert(channel->ownerLoop()==this);
	assert(isInLoopThread());
	poller_->updateChannel(channel);
}

void EventLoopff::removeChannel(Channelff* channel){
	assert(channel->ownerLoop()==this);
	assert(isInLoopThread());
	//if(EventHandling_){
			
	//}
	poller_->removeChannel(channel);
}

bool EventLoopff::hasChannel(Channelff* channel){
	assert(channel->ownerLoop()==this);
	assert(isInLoopThread());
	return poller_->hasChannel(channel);
}

//让某一线程执行某一任务
void EventLoopff::runInLoop(Functor cb){
	if(isInLoopThread()) cb();
	else{
		queueInLoop(std::move(cb));
	}
}

//将任务调度到其他线程中执行
void EventLoopff::queueInLoop(Functor cb){
	{
		MutexGuardff lock(mutex_);
		pendingFunctors_.push_back(cb);
	}

	if(!isInLoopThread()||callingPendingFunctors_){
		wakeup();
	}
}

//执行消息队列中的回调
void EventLoopff::doPendingFunctors(){
	assertInLoopThread();
	callingPendingFunctors_=true;
	std::vector<Functor> functors;
	{
		MutexGuardff lock(mutex_);
		functors.swap(pendingFunctors_);
	}
	for(auto& func:functors){
		func();
	}
	callingPendingFunctors_=false;
}

//唤醒线程和读取wakeupFd_
void EventLoopff:: wakeup(){
	uint64_t tick=1;
	ssize_t n=::write(wakeupFd_,&tick,sizeof tick);
	if(n!=sizeof tick){
		LOG_ERROR<<"EventLoop::wakeup() writes "<<n<<" bytes instead of 8";
	}
}

void EventLoopff::handleWakeupRead(){
	uint64_t tick=1;
	ssize_t n=::read(wakeupFd_,&tick,sizeof tick);
	if(n!=sizeof tick){
		LOG_ERROR<<"EventLoop::wakeup() reads "<<n<<" bytes instead of 8";
	}
}

//定时器的用户接口
TimerIdff EventLoopff::runAt(Timestampff when,timerCallback cb){
	return timerQueue_->addTimer(std::move(cb),when,0.0);
}

TimerIdff EventLoopff::runAfter(double after,timerCallback cb){
	Timestampff when(addTime(Timestampff::now(),after));
	return timerQueue_->addTimer(std::move(cb),when,0.0);
}

TimerIdff EventLoopff::runEvery(double interval,timerCallback cb){
	Timestampff when(addTime(Timestampff::now(),interval));
	return timerQueue_->addTimer(std::move(cb),when,interval);
}

void EventLoopff::cancel(TimerIdff timer){
	timerQueue_->cancel(timer);
}


