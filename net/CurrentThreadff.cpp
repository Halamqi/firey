#include "CurrentThreadff.h"

#include <type_traits>

#include <stdio.h>

using namespace firey;

namespace firey{

	namespace CurrentThreadff{

		__thread int t_cachedTid=0;
		__thread const char* t_threadName="unkown";
		__thread char t_tidString[32];
		__thread int t_tidStringLength=6;
		static_assert(std::is_same<int,pid_t>::value,"pid_t must be int");
	}//namespace CurrentThreadff

	pid_t CurrentThreadff::gettid(){
		return static_cast<pid_t>(::syscall(SYS_gettid));
	}

	void CurrentThreadff::cacheTid()
	{
		if(t_cachedTid==0)
		{
			t_cachedTid=gettid();
			t_tidStringLength=snprintf(t_tidString,sizeof t_tidString,"%5d ",t_cachedTid);
		}
	}

	bool CurrentThreadff::isMainThread()
	{
		return tid()==::getpid();
	}

}//namespace firey

