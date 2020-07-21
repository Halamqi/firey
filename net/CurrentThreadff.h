#ifndef FF_CURRENTTHREAD_H_
#define FF_CURRENTTHREAD_H_

#include <unistd.h>
#include <sys/syscall.h>


namespace firey{


	namespace CurrentThreadff{	
		extern __thread int ff_cached_tid;
		extern __thread const char* t_threadName;

		pid_t gettid();

		inline int tid(){
			if(ff_cached_tid==0){
				ff_cached_tid=static_cast<int>(gettid());
			}
			return ff_cached_tid;
		}

		inline const char* name(){
			return t_threadName;
		}

	}//namespace CurrentThreadff

}//firey

#endif //FF_CURRENTTHREAD_H_
