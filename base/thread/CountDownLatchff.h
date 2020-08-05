#ifndef FF_COUNTDOWNLATCH_H_
#define FF_COUNTDOWNLATCH_H_

#include "Conditionff.h"
#include "MutexGuardff.h"

namespace firey{

class CountDownLatchff{
	public:
		CountDownLatchff(int count);

		CountDownLatchff(const CountDownLatchff&)=delete;
		CountDownLatchff& operator=(const CountDownLatchff&)=delete;

		//等待门闩值减为0
		void wait();

		void countDown();

		int getCount() const;

	private:
		mutable Mutexff mutex_;
		Conditionff condition_;
		int count_;

};//class CountDownLatchff

}//namespace firey

#endif //FF_COUNTDOWNLATCH_H_
