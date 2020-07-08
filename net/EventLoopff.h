#ifndef FF_EVENTLOOP_H_
#define FF_EVENTLOOP_H_

#include "CurrentThreadff.h"
#include "Mutexff.h"
#include "Timerff.h"
#include "TimerIdff.h"

#include <memory>
#include <vector>
#include <functional>

namespace firey{

class Pollerff;
class Channelff;
class TimerQueueff;

class EventLoopff{
	private:
		typedef std::vector<Channelff*> ChannelList;
		ChannelList activeChannels_;

		bool looping_;
		bool quit_;
		bool eventHandling_;
		const pid_t threadId_;
		
		void abortNotInLoopThread();

		std::unique_ptr<Pollerff> poller_;

		static const int kPollWaitTime=10*1000;		

		typedef std::function<void()> Functor;
		std::vector<Functor> pendingFunctors_;

		Mutexff mutex_;
		bool callingPendingFunctors_;	
		void doPendingFunctors();

		const int wakeupFd_;
		std::unique_ptr<Channelff> wakeupChannel_;
		void handleWakeupRead();

		std::unique_ptr<TimerQueueff> timerQueue_;

	public:
		EventLoopff();
		~EventLoopff();
		EventLoopff(const EventLoopff&) = delete;
		EventLoopff& operator= (const EventLoopff&) = delete;

		bool isInLoopThread() const {return threadId_ == CurrentThreadff::tid();}

		void assertInLoopThread(){
			if(!isInLoopThread()){
				abortNotInLoopThread();
			}
		}
	public:
		void loop();
		void quit();

	public:
		void updateChannel(Channelff* channel);
		void removeChannel(Channelff* channel);
		bool hasChannel(Channelff* channel);
	
	public:
		void runInLoop(Functor cb);
		void queueInLoop(Functor cb);
		void wakeup();

	//for timer
	public:
		typedef std::function<void()> timerCallback;

		TimerIdff runAt(Timestampff when,timerCallback cb);
		TimerIdff runAfter(double after,timerCallback cb);
		TimerIdff runEvery(double interval,timerCallback cb);

		void cancel(TimerIdff timer);
};

}//namespace firey

#endif //FF_EVENTLOOP_H_
