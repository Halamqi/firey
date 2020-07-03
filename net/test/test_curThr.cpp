#include "CurrentThread.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

using namespace firey;

void* threadFunc(void* arg){
	int* i=static_cast<int*>(arg);
	printf("thread%d: pid = %d  tid = %d\n",*i,getpid(),CurrentThread::tid());
	delete i;
	return NULL;
}

int main(){
	pthread_t tid;
	for(int i=0;i<10;i++){
		int* temp=new int;
		*temp=i;
		pthread_create(&tid,NULL,threadFunc,(void*)temp);
		pthread_detach(tid);
	}
	printf("main: pid = %d  tid = %d\n",getpid(),CurrentThread::tid());	
	sleep(1);
	return 0;
}
