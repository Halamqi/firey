#include "EventLoopff.h"
#include "Channelff.h"
#include "CurrentThreadff.h"

#include <sys/timerfd.h>
#include <string.h>
#include <stdio.h>

using namespace firey;

ffEventLoop* g_loop;

void timernotify(){
	printf("thread main %d,%d timeout\n",ffCurrentThread::tid(),getpid());
	g_loop->quit();
}

int main(){
	ffEventLoop loop;
	g_loop=&loop;

	int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
	ffChannel timerChannel(&loop,timerfd);
	timerChannel.setReadCallBack(timernotify);
	timerChannel.enableReading();

	struct itimerspec howlong;
	memset(&howlong,0,sizeof howlong);
	howlong.it_value.tv_sec=5;
	::timerfd_settime(timerfd,0,&howlong,NULL);

	loop.loop();
	
	timerChannel.disableAll();
	timerChannel.remove();
	::close(timerfd);

	return 0;
}
