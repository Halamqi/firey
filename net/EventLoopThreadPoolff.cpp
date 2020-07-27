#include "EventLoopThreadPoolff.h"
#include "EventLoopThreadff.h"
#include "EventLoopff.h"

#include <stdio.h>
#include <assert.h>

using namespace firey;

EventLoopThreadPoolff::EventLoopThreadPoolff(EventLoopff* baseLoop,const std::string name)
	:baseLoop_(baseLoop),
	 name_(name),
	 started_(false),
	 numThreads_(0),
	 next_(0)
{
}

EventLoopThreadPoolff::~EventLoopThreadPoolff()
{
}

//开始运行这个IO线程池
void EventLoopThreadPoolff::start(const ThreadInitCallback& cb)
{
	assert(!started_);
	baseLoop_->assertInLoopThread();

	started_=true;

	for(int i=0;i<numThreads_;i++)
	{
		char buf[name_.size()+32]={0};
		snprintf(buf,sizeof buf,"%s%d",name_.c_str(),i+1);
		threads_.emplace_back(new EventLoopThreadff(cb,buf));
		loops_.push_back(threads_.back()->startLoop());
	}

	if(numThreads_==0&&cb)
	{
		cb(baseLoop_);
	}
}

EventLoopff* EventLoopThreadPoolff::getNextLoop()
{
	baseLoop_->assertInLoopThread();
	assert(started_);
	EventLoopff* loop=baseLoop_;

	if(!loops_.empty())
	{
		loop=loops_[next_];
		next_++;
		if(static_cast<size_t>(next_)>=loops_.size())
		{
			next_=0;
		}
	}
	return loop;
}

EventLoopff* EventLoopThreadPoolff::getLoopForHash(size_t hashCode)
{
	baseLoop_->assertInLoopThread();
	EventLoopff* loop=baseLoop_;

	if(!loops_.empty())
	{
		return loops_[hashCode%loops_.size()];
	}
	return loop;
}

std::vector<EventLoopff*> EventLoopThreadPoolff::getAllLoops()
{
	baseLoop_->assertInLoopThread();
	assert(started_);

	if(loops_.empty())
	{
		return std::vector<EventLoopff*>(1,baseLoop_);
	}
	return loops_;
}
