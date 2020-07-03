#include "Mutexff.h"
#include "CurrentThreadff.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <exception>

using namespace firey;

ffMutex::ffMutex()
	:holder_(0)
{
	if(pthread_mutex_init(&mutex_,NULL)!=0)
		throw std::exception();
}

ffMutex::~ffMutex(){
	assert(holder_==0);
	int ret=pthread_mutex_destroy(&mutex_);
	if(ret!=0) checkError(ret);
}

void ffMutex::lock(){
	int ret=pthread_mutex_lock(&mutex_);
	if(ret!=0) checkError(ret);
	holder_=ffCurrentThread::tid();
}

void ffMutex::unlock(){
	assert(holder_==ffCurrentThread::tid());
  	int ret=pthread_mutex_unlock(&mutex_);
	if(ret!=0) checkError(ret);
	holder_=0;
}

bool ffMutex::isHoldByCurrentThread(){
	return holder_==ffCurrentThread::tid();
}

void ffMutex::checkError(int check){
	fprintf(stderr,"%s,%d,%s : %s\n",__FILE__,__LINE__,__func__,strerror(check));
}
