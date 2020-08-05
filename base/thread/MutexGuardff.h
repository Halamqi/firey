#ifndef FF_MUTEXGUARD_H_
#define FF_MUTEXGUARD_H_

#include "Mutexff.h"

namespace firey{

class MutexGuardff{
	public:
		MutexGuardff(Mutexff& m)
			:mutex(m){
			mutex.lock();
			}

		~MutexGuardff(){
			mutex.unlock();
		}
		MutexGuardff(const MutexGuardff&) = delete;
		MutexGuardff& operator= (const MutexGuardff&) = delete;

	private:
		Mutexff& mutex;

};//MutexGuardff

}//namespace firey

#endif//FF_MUTEXGUARD_H_
