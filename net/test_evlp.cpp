#include "EventLoopThreadPoolff.h"
#include "EventLoopff.h"
#include "CurrentThreadff.h"

#include <stdio.h>

using namespace firey;

void callback(EventLoopff* loop){
	printf("thread %s,%d\n",CurrentThreadff::name(),CurrentThreadff::tid());
	printf("loop:%p\n",loop);
}

int main(){
	EventLoopff loop;
	EventLoopThreadPoolff evpool(&loop,"event thread pool");
	evpool.setThreadNum(10);
	evpool.start(callback);
	loop.loop();
}
