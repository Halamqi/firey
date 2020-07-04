#ifndef FF_TIMERQUEUE_H_
#define FF_TIMERQUEUE_H_

#include "Timerff.h"

#include <memory>

namespace firey{

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
		EventLoopff loop_;
		int timerFd_;
		std::unique_ptr<Channelff> timerChannel_;
	
};


}//namespace firey

#endif //FF_TIMERQUEUE_H
