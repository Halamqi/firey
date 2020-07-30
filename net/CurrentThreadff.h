#ifndef FF_CURRENTTHREAD_H_
#define FF_CURRENTTHREAD_H_

#include <unistd.h>
#include <sys/syscall.h>


namespace firey{


	namespace CurrentThreadff{	
		extern __thread int t_cachedTid;
		extern __thread const char* t_threadName;
		extern __thread char t_tidString[32];
		extern __thread int t_tidStringLength;
		pid_t gettid();

		void cacheTid();

		bool isMainThread();

		inline int tid(){
			if(__builtin_expect(t_cachedTid==0,0))
			{
				cacheTid();
			}
			return t_cachedTid;
		}

		inline const char* name(){
			return t_threadName;
		}

		inline const char* tidString()
		{
			return t_tidString;
		}

		inline int tidStringLength()
		{
			return t_tidStringLength;
		}

	}//namespace CurrentThreadff

}//firey

#endif //FF_CURRENTTHREAD_H_
