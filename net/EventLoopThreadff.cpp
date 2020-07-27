#include "EventLoopThreadff.h"
#include "EventLoopff.h"

#include <assert.h>

using namespace firey;

EventLoopThreadff::EventLoopThreadff(const ThreadInitCallback& cb,
									 const std::string name)
	 :loop_(nullptr),
	  exiting_(false),
	  thread_(std::bind(&EventLoopThreadff::threadFunc,this),name),
	  mutex_(),
	  cond_(mutex_),
	  threadInitCallback_(cb)
{
}

EventLoopThreadff::~EventLoopThreadff()
{
	exiting_=true;
	if(loop_!=nullptr)
	{
		loop_->quit();
		thread_.join();
	}
}

EventLoopff* EventLoopThreadff::startLoop()
{
	assert(!thread_.started());
	thread_.start();

	EventLoopff* loop=nullptr;
	{
		//等待IO线程的EventLoop创建好
		MutexGuardff lock(mutex_);
		while(loop_==nullptr)
		{
			cond_.wait();
		}
		loop=loop_;
	}
	return loop;
}

void EventLoopThreadff::threadFunc()
{
	//创建该线程的EventLoop
	EventLoopff loop;

	if(threadInitCallback_)
		//主要用来记录日志？
		threadInitCallback_(&loop);

	{
		MutexGuardff lock(mutex_);
		//在IO线程中创建好EventLoop之后唤醒创建这个IO线程的线p
		loop_=&loop;
		cond_.notify();
	}

	loop_->loop();

	MutexGuardff lock(mutex_);
	loop_=nullptr;
}
