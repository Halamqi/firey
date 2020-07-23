#include "Threadff.h"
#include "CurrentThreadff.h"

#include <stdio.h>
#include <vector>
#include <memory>

using namespace firey;

void tfunc1(){
	printf("this is thread %s,tid=%d\n",CurrentThreadff::t_threadName,CurrentThreadff::tid());
}

int main(){
	typedef std::vector<std::unique_ptr<Threadff>> threadList;
	threadList threads;

	for(int i=0;i<10;i++){
		threads.push_back(std::move(std::unique_ptr<Threadff>(new Threadff(tfunc1))));
		threads.back()->start();
	}
	sleep(1);
	printf("creating threads down!\n");
	return 0;
}
