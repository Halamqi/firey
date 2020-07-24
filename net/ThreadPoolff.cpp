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
		snprintf(id,sizeof id," : thread%d",i+1);
		threads_.emplace_back(new Threadff(std::bind(&ThreadPoolff::runInThread,this),name_+id));
		threads_.back()->start();
	}

	//???
	if(numThreads==0&&threadInitCallback_)
	{
		threadInitCallback_();
	}

}
