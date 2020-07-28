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
class TimerIdff;

class TimerQueueff{
	public:
		TimerQueueff(EventLoopff* loop);
		~TimerQueueff();

		TimerQueueff(const TimerQueueff&)=delete;
		TimerQueueff& operator=(const TimerQueueff&)=delete;

		typedef std::function<void()> timerCallback;
		
		TimerIdff addTimer(timerCallback cb,Timestampff when,double interval);
	
		void cancel(TimerIdff timer);

	private:
		EventLoopff* ownerLoop_;
		
		const int timerFd_;
		std::unique_ptr<Channelff> timerChannel_;
		void handleTimerfdRead();

		typedef std::pair<Timestampff,Timerff*> TimerNode;
		typedef std::set<TimerNode> TimerTree;
		TimerTree timers_;

		typedef std::pair<Timerff*,uint64_t> ActiveTimer;
		typedef std::set<ActiveTimer> activeTimerSet;
		activeTimerSet activeTimers_;
		
		bool callingExpiredTimers_;

		void addTimerInLoop(Timerff* timer);

		std::vector<TimerNode> getExpiredTimer(Timestampff now);
		void reset(const std::vector<TimerNode>& expired,Timestampff now);

		bool insertTimer(Timerff* timer);

		//为了防止待取消的定时器到期正在执行，在两个定时器集合中找不到，
		//所以将其添加到待取消的定时器，待到期定时器执行完毕以后，判断是否在待取消的定时器队列中，
		//如果在待取消的定时器队列中，即便是重复的定时器也不重置它
		std::set<ActiveTimer> cancelingTimers_;
		void cancelInLoop(TimerIdff timerId);
};


}//namespace firey

#endif //FF_TIMERQUEUE_H
