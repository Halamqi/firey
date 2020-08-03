#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


#include <queue>
#include <functional>
#include <cassert>

class ThreadPool{
	public:
		typedef std::function<void()> Task;
		ThreadPool(int threadNum,int maxQueueSize);
		~ThreadPool();

		void start();

		void run(Task);

		int threadNum(){return threadNum_;}
		void setMaxQueueSize(int maxQueueSize){maxQueueSize_=maxQueueSize;}

	private:
		int threadNum_;
		int maxQueueSize_;
		pthread_t* threads_;

		std::deque<Task> queue_;

		pthread_mutex_t mutex_;
		pthread_cond_t notEmpty_;
		pthread_cond_t notFull_;

		bool running_;

		Task take();
		static void* runInThread(void*);
		bool isFull();
};//class ThreadPool

ThreadPool::ThreadPool(int threadNum,int maxQueueSize)
	:threadNum_(threadNum),
	maxQueueSize_(maxQueueSize),
	threads_(new pthread_t[threadNum_]),
	running_(false)
{
	if(!threads_){
		fprintf(stderr,"ThreadPool::ThreadPool() failed\n");
		abort();
	}
	mutex_=PTHREAD_MUTEX_INITIALIZER;
	notEmpty_=PTHREAD_COND_INITIALIZER;
	notFull_=PTHREAD_COND_INITIALIZER;
}

ThreadPool::~ThreadPool()
{
	running_=false;
	pthread_cond_broadcast(&notFull_);
	pthread_cond_broadcast(&notEmpty_);
	for(int i=0;i<threadNum_;i++)
	{
		pthread_join(threads_[i],NULL);
	}

	if(threads_){
		delete[] threads_;
	}
}

void* ThreadPool::runInThread(void* arg)
{
	ThreadPool* pool=static_cast<ThreadPool*>(arg);
	assert(pool!=nullptr);
	while(pool->running_)
	{
		Task task=pool->take();
		if(task){
			task();
		}
	}
	return pool;
}

void ThreadPool::start()
{
	assert(!running_);
	running_=true;
	for(int i=0;i<threadNum_;i++)
	{
		if(pthread_create(&threads_[i],NULL,runInThread,this))
		{
			fprintf(stderr,"pthread_create() failed\n");
			abort();
		}
	}
}

bool ThreadPool::isFull(){
	return maxQueueSize_>0&&static_cast<ssize_t>(queue_.size())>=maxQueueSize_;
}

void ThreadPool::run(Task task)
{
	if(threadNum_<=0)
	{
		task();
	}
	else
	{
		pthread_mutex_lock(&mutex_);
		while(isFull()&&running_)
		{
			pthread_cond_wait(&notFull_,&mutex_);
		}
		if(!running_) return;
		assert(!isFull());
		queue_.push_back(task);
		pthread_cond_signal(&notEmpty_);
		pthread_mutex_unlock(&mutex_);
	}
}

ThreadPool::Task ThreadPool::take()
{
	pthread_mutex_lock(&mutex_);
	while(queue_.empty()&&running_)
	{
		pthread_cond_wait(&notEmpty_,&mutex_);
	}
	
	Task task;
	if(!queue_.empty())
	{
		task=queue_.front();
		queue_.pop_front();
		if(maxQueueSize_>0)
		{
			pthread_cond_signal(&notFull_);
		}
	}
	pthread_mutex_unlock(&mutex_);
	return task;
}

#endif //THREADPOOL_H_
