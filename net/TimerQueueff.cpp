#include "Timestampff.h"
#include "Channelff.h"
#include "EventLoopff.h"
#include "Timerff.h"
#include "TimerIdff.h"

#include <sys/timerfd.h>
#include <unistd.h>

using namespace firey;

int createTimerfd(){
	int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
	if(timerfd<0) abort();
	return timerfd;
}

struct timespec howMuchTimerFromNow(Timestampff when){
	int64_t when.usSinceEpoch()-Timestampff::now().usSinceEpoch();
}
TimerQueueff::TimerQueueff(EventLoopff* loop)
	:ownerLoop_(loop),
	timerFd_(createTimerfd()),
	timerChannel_(new Channelff(ownerLoop_,timerFd_)),
	callingExpiredTimers_(false)
{
	timerChannel_->setReadCallback(
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

TimerIdff addTimer(TimerCallback cb,Timestamff when,double interval){
	Timerff* timer=new Timerff(std::move(cb),when,interval);
	ownerLoop_->runInLoop(
			std::bind(TimerQueueff::addTimerInLoop,this,timer));
	return TimerIdff(timer,timer->sequence());
}

void addTimerInLoop(Timerff* timer){
	ownerLoop_->assertInLoopThread();

	bool earliestChanged = insertTimer(timer);

	if(earliestChanged){
		resetTimerfd(timerFd_,timer->expireTime());
	}
}
