#ifndef FF_TIMER_H_
#define FF_TIMER_H_

#include "Timestampff.h"

#include <atomic>
#include <functional>

namespace firey{

class Timerff{
	public:
		typedef std::function<void()> timerCallback;
		
		Timerff(timerCallback cb,Timestampff when,double interval);
		~Timerff();

		Timerff(const Timerff&)=delete;
		Timerff& operator=(const Timerff&)=delete;
	
		void run() const{
			callback_();
		}

		Timestampff expireTime() const {return expireTime_;}
		bool isRepeat() const {return isRepeat_;}
		double interval() const {return interval_;}
		uint64_t sequence() const {return sequence_;}
		
		//restart a repeat timer
		void restart(Timestampff now);
		
		static uint64_t timerCreated(){return timerCreated_;}

	private:
		//timer's callback
		timerCallback callback_;
		double interval_;
		bool isRepeat_;
		Timestampff expireTime_;
		const uint64_t sequence_;//this timer's sequence(unique)

		//timer created totally
		static std::atomic<uint64_t> timerCreated_;
};//class Timerff


}//namespace firey

#endif //FF_TIMER_H_
