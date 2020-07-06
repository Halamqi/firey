#ifndef FF_TIMERQUEUE_H_
#define FF_TIMERQUEUE_H_

#include "Timerff.h"//for timerCallback

#include <memory>
#include <vector>
#include <set>

namespace firey{

class Timestampff;
class Channelff;
class EventLoopff;

class TimerQueueff{
	public:
		TimerQueueff(EventLoopff* loop);
		~TimerQueueff();
		
		TimerQueueff(const TimerQueueff&)=delete;
		TimerQueueff& operator=(const TimerQueueff&)=delete;

		void addTimer(TimerCallback cb,Timestampff when,double interval);
	
		void cancel(ffTimerId& timer);

	private:
		EventLoopff ownerLoop_;
		
		const int timerFd_;
		std::unique_ptr<Channelff> timerChannel_;
		void handleTimerfdRead();

		typedef std::pair<Timestampff,Timerff*> TimerNode;
		typedef std::set<TimerNode> TimerTree;
		TimerTree timers_;

		void addTimerInLoop(Timerff* timer);
		
		bool callingExpiredTimers_;
		std::vector<TimerNode> getExpired(Timestampff now);
		
		bool insertTimer(Timerff* timer);

};


}//namespace firey

#endif //FF_TIMERQUEUE_H
