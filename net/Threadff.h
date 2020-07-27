#ifndef FF_THREAD_H_
#define FF_THREAD_H_

#include "CountDownLatchff.h"

#include <string>
#include <pthread.h>
#include <functional>
#include <memory>
#include <atomic>

namespace firey{

class Threadff
{
	public:
		typedef std::function<void()> ThreadFunc;

		explicit Threadff(ThreadFunc,const std::string& name=std::string());
		~Threadff();

		Threadff(const Threadff&)=delete;
		Threadff& operator=(const Threadff&)=delete;

		void start();
		int join();
		
		bool started() const{return started_;}
		pid_t tid() const{return tid_;}
		const std::string& name() const{return name_;}

		static uint32_t threadCreated() {return threadCreated_;}
	private:
		void setDefaultName();

		bool started_;
		bool joined_;
		
		pid_t tid_;
		pthread_t threadId_;
		ThreadFunc func_;
		std::string name_;
		//等待线程创建完毕之后，再继续运行主线程
		CountDownLatchff latch_;

		static std::atomic<uint32_t> threadCreated_; 	
};//class Threadff

}//namespace firey

#endif //FF_THREAD_H_
