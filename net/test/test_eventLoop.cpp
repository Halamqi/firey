#include "ffEventLoop.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

//firey::ffEventLoop* ptr_loop;

void* threadFunc(void* ){
	printf("threadFunc(): pid = %d, tid = %d\n",getpid(),firey::CurrentThread::tid());
	return NULL;
}

int main(){
	printf("main(): pid = %d, tid = %d\n",getpid(),firey::CurrentThread::tid());
	firey::ffEventLoop loop;
	firey::ffEventLoop loop1;
	//ptr_loop=&loop;
	pthread_t tid;
	pthread_create(&tid,NULL,threadFunc,NULL);
	pthread_detach(tid);

	pthread_exit(NULL);
}
