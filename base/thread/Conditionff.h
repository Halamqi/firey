#ifndef FF_CONDITION_H_
#define FF_CONDITION_H_

#include "Mutexff.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

namespace firey{
class Conditionff{
	public:
		Conditionff(Mutexff& m)
			:mutex_(m)
		{
			int ret=pthread_cond_init(&cond_,NULL);
			if(ret!=0) checkError(ret);
		}

		~Conditionff(){
			int ret=pthread_cond_destroy(&cond_);
			if(ret!=0) checkError(ret);
		}

		void wait(){
			int ret=pthread_cond_wait(&cond_,mutex_.getPthreadMutex());
			if(ret!=0) checkError(ret);
		}

		bool waitForSeconds(double second){
			struct timespec abstime;
			clock_gettime(CLOCK_REALTIME,&abstime);

			const int64_t kNanoSecondsPerSecond=1000*1000*1000;
			int64_t nanoseconds=static_cast<int64_t>(second * kNanoSecondsPerSecond);

			abstime.tv_sec+=static_cast<time_t>((abstime.tv_nsec+nanoseconds)/kNanoSecondsPerSecond);
			abstime.tv_nsec+=static_cast<long>((abstime.tv_nsec+nanoseconds)%kNanoSecondsPerSecond);

			return ETIMEDOUT==pthread_cond_timedwait(&cond_,mutex_.getPthreadMutex(),&abstime);
		}

		void notify(){
			int ret=pthread_cond_signal(&cond_);
			if(ret!=0) checkError(ret);
		}

		void notifyAll(){
			int ret=pthread_cond_broadcast(&cond_);
			if(ret!=0) checkError(ret);
		}

	private:
		pthread_cond_t cond_;
		Mutexff& mutex_;
		void checkError(int check){
			fprintf(stderr,"%s,%d,%s : %s",__FILE__,__LINE__,__func__,strerror(check));
		}

};//Conditionff

}//namespace firey

#endif //FF_CONDITION_H_
