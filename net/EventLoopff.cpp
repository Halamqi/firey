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

__thread ffEventLoop* t_loopInThisThread=nullptr;

void ffEventLoop::abortNotInLoopThread(){
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

ffEventLoop::ffEventLoop()
	:looping_(false),
	quit_(false),
	eventHandling_(false),
	threadId_(ffCurrentThread::tid()),
	poller_(new ffPoller(this)),
	callingPendingFunctors_(false),
	wakeupFd_(createEventFd()),
	wakeupChannel_(new ffChannel(this,wakeupFd_))
{
	assert(!t_loopInThisThread);
	t_loopInThisThread=this;

	wakeupChannel_->setReadCallBack(
			std::bind(&ffEventLoop::handleWakeupRead,this));
	wakeupChannel_->enableReading();
}

ffEventLoop::~ffEventLoop(){
	assert(!looping_);
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
	t_loopInThisThread=nullptr;
}

void ffEventLoop::loop(){
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

void ffEventLoop::quit(){
	assert(!quit_);
	quit_=true;
}

void ffEventLoop::updateChannel(ffChannel* channel){
	assert(channel->ownerLoop()==this);
	assert(isInLoopThread());
	poller_->updateChannel(channel);
}

void ffEventLoop::removeChannel(ffChannel* channel){
	assert(channel->ownerLoop()==this);
	assert(isInLoopThread());
	//if(EventHandling_){
		
	//}
	poller_->removeChannel(channel);
}

bool ffEventLoop::hasChannel(ffChannel* channel){
	assert(channel->ownerLoop()==this);
	assert(isInLoopThread());
	return poller_->hasChannel(channel);
}

void ffEventLoop::runInLoop(Functor cb){
	if(isInLoopThread()) cb();
	else{
		queueInLoop(std::move(cb));
	}
}

void ffEventLoop::queueInLoop(Functor cb){
	{
		ffMutexGuard lock(mutex_);
		pendingFunctors_.push_back(cb);
	}

	if(!isInLoopThread()||callingPendingFunctors_){
		wakeup();
	}
}


void ffEventLoop::doPendingFunctors(){
	assertInLoopThread();
	callingPendingFunctors_=true;
	std::vector<Functor> functors;
	{
		ffMutexGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}
	for(auto& func:functors){
		func();
	}
	callingPendingFunctors_=false;
}

void ffEventLoop:: wakeup(){
	uint64_t tick=1;
	ssize_t n=::write(wakeupFd_,&tick,sizeof tick);
	if(n!=sizeof tick){
		fprintf(stderr,"ffEventLoop::wakeup() wakeup failed\n");
	}
}

void ffEventLoop::handleWakeupRead(){
	uint64_t tick=1;
	ssize_t n=::read(wakeupFd_,&tick,sizeof tick);
	if(n!=sizeof tick){
		fprintf(stderr,"ffEventLoop::wakeup() wakeup failed\n");
	}
}
