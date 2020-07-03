#ifndef FF_EVENTLOOP_H_
#define FF_EVENTLOOP_H_

#include "CurrentThreadff.h"
#include "Mutexff.h"

#include <memory>
#include <vector>
#include <functional>

namespace firey{

class ffPoller;
class ffChannel;

class ffEventLoop{
	private:
		typedef std::vector<ffChannel*> ChannelList;
		ChannelList activeChannels_;

		bool looping_;
		bool quit_;
		bool eventHandling_;
		const pid_t threadId_;
		
		void abortNotInLoopThread();

		std::unique_ptr<ffPoller> poller_;

		static const int kPollWaitTime=10*1000;		

		typedef std::function<void()> Functor;
		std::vector<Functor> pendingFunctors_;
		ffMutex mutex_;
		bool callingPendingFunctors_;	
		void doPendingFunctors();

		const int wakeupFd_;
		std::unique_ptr<ffChannel> wakeupChannel_;
		void handleWakeupRead();

	public:
		ffEventLoop();
		~ffEventLoop();
		ffEventLoop(const ffEventLoop&) = delete;
		ffEventLoop& operator= (const ffEventLoop&) = delete;

		bool isInLoopThread() const {return threadId_ == ffCurrentThread::tid();}

		void assertInLoopThread(){
			if(!isInLoopThread()){
				abortNotInLoopThread();
			}
		}
	public:
		void loop();
		void quit();

	public:
		void updateChannel(ffChannel* channel);
		void removeChannel(ffChannel* channel);
		bool hasChannel(ffChannel* channel);
	
	public:
		void runInLoop(Functor cb);
		void queueInLoop(Functor cb);
		void wakeup();

};

}//namespace firey

#endif //FF_EVENTLOOP_H_
