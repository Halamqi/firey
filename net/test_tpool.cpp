#include "ThreadPoolff.h"
#include "CurrentThreadff.h"

#include "stdio.h"

using namespace firey;

void threadInitCallback(){
	printf("this is %s,%d\n",CurrentThreadff::name(),CurrentThreadff::tid());
}

int main()
{
	ThreadPoolff thread_pool("test thread pool:");
	thread_pool.setThreadInitCallback(threadInitCallback);
	thread_pool.setMaxQueueSize(10);
	thread_pool.start(10);
	return 0;
}
