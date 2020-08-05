#include "CurrentThreadff.h"
#include "Timerff.h"

#include <vector>
#include <memory>

#include <stdio.h>
#include <pthread.h>

using namespace firey;
using namespace std;

void timercb(){
	printf("timercall...");	
}

void* threadFunc(void* arg){
	vector<std::unique_ptr<Timerff>> timers;
	for(int i=0;i<10;i++){
		timers.push_back(unique_ptr<Timerff>(new Timerff(timercb,addTime(Timestampff::now(),static_cast<double>(i+1)),0.0)));
		printf("this is in %d:\n",CurrentThreadff::tid());
		printf("timer created:%lu sequence:%lu\n",Timerff::timerCreated(),timers[i]->sequence());
	}		
	return NULL;
}


int main(){
	vector<std::unique_ptr<Timerff>> timers;
	pthread_t tid;
	for(int i=0;i<1;i++){
		pthread_create(&tid,NULL,threadFunc,NULL);
		pthread_detach(tid);
	}

	for(int i=0;i<10;i++){
		timers.push_back(unique_ptr<Timerff>(new Timerff(timercb,addTime(Timestampff::now(),static_cast<double>(i+1)),0.0)));
		printf("this is in %d:\n",CurrentThreadff::tid());
		printf("timer created:%lu sequence:%lu\n",Timerff::timerCreated(),timers[i]->sequence());
	}

	sleep(5);
	return 0;
}
