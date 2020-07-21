#include "CurrentThreadff.h"

using namespace firey;

namespace firey{

	namespace CurrentThreadff{

		__thread int ff_cached_tid=0;
		__thread const char* t_threadName="unkown";

		pid_t gettid(){
			return static_cast<pid_t>(::syscall(SYS_gettid));
		}

	}//namesapce CurrentThreadff

}//namespace firey

