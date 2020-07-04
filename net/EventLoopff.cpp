#include "EventLoopff.h"
#include "Pollerff.h"
#include "Channelff.h"
#include "MutexGuardff.h"

#include <assert.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <stdio.h>

using namespace firey;

__thread EventLoopff* t_loopInThisThread=nullptr;

void EventLoopff::abortNotInLoopThread(){
	assert(isInLoopThread());
}

int createEventFd(){
	int evfd=::eventfd(0,EFD_CLOEXEC|EFD_NONBLOCK);
	if(evfd<0) abort();
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
	wakeupChannel_(new Channelff(this,wakeupFd_))
{
	assert(!t_loopInThisThread);
	t_loopInThisThread=this;

	wakeupChannel_->setReadCallBack(
			std::bind(&EventLoopff::handleWakeupRead,this));
	wakeupChannel_->enableReading();
}

EventLoopff::~EventLoopff(){
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
	while(!quit_){
		activeChannels_.clear();
		poller_->poll(kPollWaitTime,&activeChannels_);

		eventHandling_=true;
		for(auto ach:activeChannels_){
			ach->handleEvent();
		}
		eventHandling_=false;
		
		doPendingFunctors();
	}
	looping_=false;

}

void EventLoopff::quit(){
	assert(!quit_);
	quit_=true;
}

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

void EventLoopff::runInLoop(Functor cb){
	if(isInLoopThread()) cb();
	else{
		queueInLoop(std::move(cb));
	}
}

void EventLoopff::queueInLoop(Functor cb){
	{
		MutexGuardff lock(mutex_);
		pendingFunctors_.push_back(cb);
	}

	if(!isInLoopThread()||callingPendingFunctors_){
		wakeup();
	}
}


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

void EventLoopff:: wakeup(){
	uint64_t tick=1;
	ssize_t n=::write(wakeupFd_,&tick,sizeof tick);
	if(n!=sizeof tick){
		fprintf(stderr,"EventLoopff::wakeup() wakeup failed\n");
	}
}

void EventLoopff::handleWakeupRead(){
	uint64_t tick=1;
	ssize_t n=::read(wakeupFd_,&tick,sizeof tick);
	if(n!=sizeof tick){
		fprintf(stderr,"EventLoopff::wakeup() wakeup failed\n");
	}
}
