#ifndef FF_CURRENTTHREAD_H_
#define FF_CURRENTTHREAD_H_

#include <unistd.h>
#include <sys/syscall.h>

static __thread int cached_tid=0;

namespace firey{
class CurrentThreadff{
	public:	
	static pid_t gettid(){
		return static_cast<pid_t>(::syscall(SYS_gettid));
	}

	static int tid(){
		if(cached_tid==0){
			cached_tid=static_cast<int>(gettid());
		}
		return cached_tid;
	}
};//CurrentThread
}//firey

#endif //FF_CURRENTTHREAD_H_
