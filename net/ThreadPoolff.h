#ifndef FF_THREADPOOL_H_
#define FF_THREADPOOL_H_

#include "MutexGuardff.h"
#include "Conditionff.h"
#include "Threadff.h"


#include <functional>
#include <deque>
#include <vector>
#include <string>

namespace firey
{

class ThreadPoolff
{
	public:
		typedef std::function<void()> Task;

		explicit ThreadPoolff(const std::string& name=std::string("ThreadPool"));
		~ThreadPoolff();

		ThreadPoolff(const ThreadPoolff&)=delete;
		ThreadPoolff& operator=(const ThreadPoolff&)=delete;

		void setMaxQueueSize(size_t maxSize){maxQueueSize_=maxSize;}
		
		//???
		void setThreadInitCallback(const Task& t)
		{
			threadInitCallback_=t;
		}

		void start(int numThreads);
		void stop();

		const std::string& name() const {return name_;}

		size_t queueSize() const;

		void run(Task f);

	private:
		bool isFull() const;
		void runInThread();
		//取走任务
		Task take();
		//任务队列
		std::deque<Task> queue_;
		mutable Mutexff mutex_;
		Conditionff notFull_;
		Conditionff notEmpty_;
		
		std::string name_;
		//？？
		Task threadInitCallback_;
		//线程池中的各个线程
		std::vector<std::unique_ptr<Threadff>> threads_;
		//阻塞队列的最大长度
		size_t maxQueueSize_;
		bool running_;
};//class ThreadPoolff

};//namespace firey

#endif //FF_THREADPOOL_H_
