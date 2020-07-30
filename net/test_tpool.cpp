#include "ThreadPoolff.h"
#include "CurrentThreadff.h"

#include "stdio.h"

using namespace firey;

int number=0;

Mutexff mutex_;

void threadInitCallback(){
	printf("this is %s,%d\n",CurrentThreadff::name(),CurrentThreadff::tid());
}

void task(){
	MutexGuardff lock(mutex_);
	number++;
	printf("task%d\n",number);
}

int main()
{
	ThreadPoolff thread_pool("test thread pool");
	thread_pool.setThreadInitCallback(threadInitCallback);
	thread_pool.setMaxQueueSize(10);
	thread_pool.start(10);
/*	for(int i=0;i<30000;i++){
		thread_pool.run(task);
	}
	*/
	sleep(1);
	return 0;
}
