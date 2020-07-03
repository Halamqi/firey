#ifndef FF_MUTEXGUARD_H_
#define FF_MUTEXGUARD_H_

#include "Mutexff.h"

namespace firey{

class ffMutexGuard{
	public:
		ffMutexGuard(ffMutex& m)
			:mutex(m){
			mutex.lock();
			}

		~ffMutexGuard(){
			mutex.unlock();
		}
		ffMutexGuard(const ffMutexGuard&) = delete;
		ffMutexGuard& operator= (const ffMutexGuard&) = delete;

	private:
		ffMutex& mutex;

};//ffMutexGuard

}//namespace firey

#endif//FF_MUTEXGUARD_H_
