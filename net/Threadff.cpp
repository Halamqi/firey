#include "Threadff.h"
#include "CurrentThreadff.h"

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <linux/unistd.h>

using namespace firey;

void afterFork()
{
	CurrentThreadff::t_threadName="main";
	CurrentThreadff::tid();
}
//RAII在主线程启动的时候，就初始化主线程的信息
class ThreadNameInitializer
{
	public:
		ThreadNameInitializer()
		{
			CurrentThreadff::t_threadName="main";
			CurrentThreadff::tid();
			pthread_atfork(NULL,NULL,afterFork);
		}
};

ThreadNameInitializer Init;

struct ThreadData
{

	typedef Threadff::ThreadFunc ThreadFunc;
	ThreadFunc func_;//线程的主函数
	std::string name_;//线程名称
	pid_t* tid_;//线程的标识id
	CountDownLatchff* latch_;//线程启动门闩

	ThreadData(ThreadFunc func,
			   const std::string& name,
			   pid_t* tid,
			   CountDownLatchff* latch)
		:func_(std::move(func)),
		 name_(name),
		 tid_(tid),
		 latch_(latch)
	{
	}
	
	//线程执行用户传入线程的线程数据中的函数回调
	void runInThread()
	{
		//线程开始运行时，将线程的tid传递给启动该线程的线程
		//并将启动门闩减一，标识该线程正在初始化
		*tid_=CurrentThreadff::tid();
		tid_=nullptr;
		latch_->countDown();
		latch_=nullptr;

		//初始化线程的名称
		CurrentThreadff::t_threadName=name_.empty()?"fireyThread":name_.c_str();
		::prctl(PR_SET_NAME,CurrentThreadff::t_threadName);
		
		//去执行用户设置的回调。
		try
		{
			func_();
			CurrentThreadff::t_threadName="finish";
		}
		catch(const std::exception& ex)
		{
			CurrentThreadff::t_threadName="crashed";
			fprintf(stderr,"exception caught in Thread %s\n",name_.c_str());
			fprintf(stderr,"reason: %s\n",ex.what());
			abort();
		}
		catch(...)
		{
			CurrentThreadff::t_threadName="crashed";
			fprintf(stderr,"unkown exception caught in Thread %s\n",name_.c_str());
			throw;
		}
	}
};

//每个线程真正的入口函数
void* startThread(void* arg)
{
	//接受用户传来的线程数据（包括需要设置的线程tid，线程名称，门闩及最终要的用户需要线程区执行的函数回调）
	ThreadData* data=static_cast<ThreadData*>(arg);
	//在线程中执行线程数据中的runInThread-->进而执行用户设置好的回调函数func_
	data->runInThread();
	delete data;
	return nullptr;
}

std::atomic<uint32_t> Threadff::threadCreated_(0);

Threadff::Threadff(ThreadFunc func,const std::string& name)
	:started_(false),
	 joined_(false),
	 tid_(0),
	 threadId_(0),
	 func_(std::move(func)),
	 name_(name),
	 latch_(1)
{
	setDefaultName();
}

Threadff::~Threadff()
{
	if(!joined_&&started_){
		pthread_detach(threadId_);
	}
}

void Threadff::setDefaultName()
{
	uint32_t num=++threadCreated_;
	if(name_.empty()){
		char buf[32];
		snprintf(buf,sizeof buf,"Thread%d",num);
		name_=buf;
	}
}


void Threadff::start()
{
	assert(!started_);
	started_=true;
	ThreadData* data=new ThreadData(func_,name_,&tid_,&latch_);
	if(pthread_create(&threadId_,NULL,startThread,data))
	{
		started_=false;
		delete data;
		//TODO LOG_SYSFATAL<<
	}
	//等待线程创建完毕，阻塞等待
	else
	{
		latch_.wait();
		assert(tid_>0);
	}
}

int Threadff::join()
{
	assert(started_);
	assert(!joined_);
	joined_=true;
	return pthread_join(threadId_,NULL);
}
