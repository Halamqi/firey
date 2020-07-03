#ifndef FF_CONDITION_H_
#define FF_CONDITION_H_

#include "Mutexff.h"
#include <stdio.h>
#include <string.h>

namespace firey{
class ffCondition{
	public:
		ffCondition(Mutex& m)
			:mutex_(m)
		{
			int ret=pthread_cond_init(&cond_,NULL);
			if(ret!=0) checkError(ret);
		}

		~ffCondition(){
			int ret=pthread_cond_destroy(&cond_);
			if(ret!=0) checkError(ret);
		}

		void wait(){
			int ret=pthread_cond_wait(&cond_,mutex_.getPthreadMutex());
			if(ret!=0) checkError(ret);
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
		ffMutex& mutex_;
		void checkError(int check){
			fprintf(stderr,"%s,%d,%s : %s",__FILE__,__LINE__,__func__,strerror(check));
		}

};//ffCondition

}//namespace firey

#endif //FF_CONDITION_H_
