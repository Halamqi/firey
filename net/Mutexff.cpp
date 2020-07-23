#include "Mutexff.h"
#include "CurrentThreadff.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <exception>

using namespace firey;

Mutexff::Mutexff()
{
	if(pthread_mutex_init(&mutex_,NULL)!=0)
		throw std::exception();
}

Mutexff::~Mutexff(){
	int ret=pthread_mutex_destroy(&mutex_);
	if(ret!=0) checkError(ret);
}

void Mutexff::lock(){
	int ret=pthread_mutex_lock(&mutex_);
	if(ret!=0) checkError(ret);
}

void Mutexff::unlock(){
  	int ret=pthread_mutex_unlock(&mutex_);
	if(ret!=0) checkError(ret);
}


void Mutexff::checkError(int check){
	fprintf(stderr,"%s,%d,%s : %s\n",__FILE__,__LINE__,__func__,strerror(check));
}
