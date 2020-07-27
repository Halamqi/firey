#ifndef FF_EVENTLOOPTHREAD_H_
#define FF_EVENTLOOPTHREAD_H_

#include "Threadff.h"
#include "MutexGuardff.h"
#include "Conditionff.h"

#include <string>

namespace firey{

class EventLoopff;

//创建一个IO线程
class EventLoopThreadff{

	public:
		typedef std::function<void(EventLoopff*)> ThreadInitCallback;
		
		EventLoopThreadff(const ThreadInitCallback& cb=ThreadInitCallback(),
						  const std::string name=std::string());
		~EventLoopThreadff();

		EventLoopThreadff(const EventLoopThreadff&)=delete;
		EventLoopThreadff& operator=(const EventLoopThreadff&)=delete;

		EventLoopff* startLoop();

	private:
		void threadFunc();

		EventLoopff* loop_;
		bool exiting_;
		Threadff thread_;

		//用来对IO线程的EventLoop进行同步
		Mutexff mutex_;
		Conditionff cond_;
		ThreadInitCallback threadInitCallback_;
};//class EventLoopThreadff

}//namespace firey 

#endif //FF_EVENTLOOPTHREAD_H_
