#ifndef FF_EVENTLOOPTHREADPOOL_H_
#define FF_EVENTLOOPTHREADPOOL_H_

#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace firey{

class EventLoopff;
class EventLoopThreadff;

class EventLoopThreadPoolff{
	public:
		typedef std::function<void(EventLoopff*)> ThreadInitCallback;

		EventLoopThreadPoolff(EventLoopff* baseLoop,
							  const std::string name=std::string());
		~EventLoopThreadPoolff();

		EventLoopThreadPoolff(const EventLoopThreadPoolff&)=delete;
		EventLoopThreadPoolff& operator=(const EventLoopThreadPoolff&)=delete;

		void setThreadNum(int numThread){numThreads_=numThread;}
		void start(const ThreadInitCallback& cb=ThreadInitCallback());

		EventLoopff* getNextLoop();

		EventLoopff* getLoopForHash(size_t hashCode);

		std::vector<EventLoopff*> getAllLoops();

		bool started() const{return started_;}
		const std::string& name() const{return name_;}

	private:
		EventLoopff* baseLoop_;
		std::string name_;
		bool started_;
		int numThreads_;
		int next_;
		//线程池中的所有IO线程
		std::vector<std::unique_ptr<EventLoopThreadff>> threads_;
		std::vector<EventLoopff*> loops_;

};//class EventLoopThreadPoolff

}//namespace firey

#endif//FF_EVENTLOOPTHREADPOOL_H_
