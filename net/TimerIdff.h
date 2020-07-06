#ifndef FF_TIMERID_H_
#define FF_TIMERID_H_

namespace firey{

class Timerff;

class TimerIdff{

	public:
		TimerIdff(Timerff* timer,uint64_t seq)
			:timer_(timer),
			sequence_(seq)
		{
		}

		TimerIdff()
			:timer_(nullptr),
			sequence_(0)
		{
		}
	
	private:
		Timerff* timer_;
		uint64_t sequence_;

};//class TimerffId

}//namespace firey

#endif //FF_TIMERID_H_
