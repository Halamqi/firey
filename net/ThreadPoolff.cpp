#include "ThreadPoolff.h"

#include <assert.h>
#include <stdio.h>

using namespace firey;

ThreadPoolff::ThreadPoolff(const std::string& name)
	:mutex_(),
	 notFull_(mutex_),
	 notEmpty_(mutex_),
	 name_(name),
	 maxQueueSize_(0),
	 running_(false)
{}

ThreadPoolff::~ThreadPoolff()
{
	if(running_)
	{
		stop();
	}
}

void ThreadPoolff::start(int numThreads)
{
	assert(threads_.empty());
	running_=true;
	threads_.reserve(numThreads);
	for(int i=0;i<numThreads;i++)
	{
		char id[32]={0};
		snprintf(id,sizeof id,":%d",i+1);
		threads_.emplace_back(new Threadff(std::bind(&ThreadPoolff::runInThread,this),name_+id));
		threads_.back()->start();
	}

	//???
	if(numThreads==0&&threadInitCallback_)
	{
		threadInitCallback_();
	}

}

void ThreadPoolff::stop()
{
	{
		MutexGuardff lock(mutex_);
		running_=false;
		notEmpty_.notifyAll();
		notFull_.notifyAll();
	}

	//join掉各个线程
	for(auto& thread:threads_){
		thread->join();
	}
}

size_t ThreadPoolff::queueSize() const{
	MutexGuardff lock(mutex_);
	return queue_.size();
}

//让线程池执行一个任务
void ThreadPoolff::run(Task task)
{
	if(threads_.empty())
		task();
	else
	{
		MutexGuardff lock(mutex_);
		while(isFull()&&running_)
		{
			notFull_.wait();
		}
		if(!running_) return;
		assert(!isFull());

		queue_.push_back(std::move(task));
		notEmpty_.notify();
	}
}

ThreadPoolff::Task ThreadPoolff::take()
{
	MutexGuardff lock(mutex_);
	while(queue_.empty()&&running_)
	{
		notEmpty_.wait();
	}

	Task task;
	if(!queue_.empty())
	{
		task=queue_.front();
		queue_.pop_front();
		if(maxQueueSize_>0)
			notFull_.notify();
	}
	return task;
}

bool ThreadPoolff::isFull() const
{
	return maxQueueSize_>0&&queue_.size()>=maxQueueSize_;
}

//线程池中的线程要执行的工作
void ThreadPoolff::runInThread()
{
	try
	{
		if(threadInitCallback_)
			threadInitCallback_();
		while(running_)
		{
			Task task(take());
			if(task)
			{
				task();
			}
		}
	}

	catch(const std::exception& ex)
	{
		fprintf(stderr,"exception caught in ThreadPool %s\n",name_.c_str());
		fprintf(stderr,"reason: %s\n",ex.what());
		abort();
	}
	catch(...)
	{
		fprintf(stderr,"unknown exception caught in ThreadPool %s\n",name_.c_str());
		throw;
	}
}
