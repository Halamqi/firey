#include "CountDownLatchff.h"

using namespace firey;

CountDownLatchff::CountDownLatchff(int count)
	:mutex_(),
	condition_(mutex_),
	count_(count)
{

}

void CountDownLatchff::wait()
{
	{
		MutexGuardff lock(mutex_);
		while(count_>0){
			condition_.wait();
		}
	}
}

void CountDownLatchff::countDown()
{
	{
		MutexGuardff lock(mutex_);
		count_--;
		if(count_==0)
		{
			condition_.notifyAll();
		}
	}
}

int CountDownLatchff::getCount() const
{
	MutexGuardff lock(mutex_);
	return count_;
}
