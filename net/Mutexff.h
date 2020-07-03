#ifndef FF_MUTEX_H_
#define FF_MUTEX_H_
#include <pthread.h>

namespace firey{

class ffMutex{
public:
	ffMutex();
	~ffMutex();
	ffMutex(const ffMutex&) = delete;
	ffMutex& operator=(const ffMutex&) = delete;

	void lock();
	void unlock();
	inline pthread_mutex_t* getPthreadMutex(){
		return &mutex_;
	}
	bool isHoldByCurrentThread();
private:
	pthread_mutex_t mutex_;
	pid_t holder_;

	void checkError(int);
};//Mutex

}//namespace firey

#endif//MUTEX_H_
