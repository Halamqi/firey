#ifndef FF_MUTEX_H_
#define FF_MUTEX_H_
#include <pthread.h>

namespace firey{

class Mutexff{
public:
	Mutexff();
	~Mutexff();
	Mutexff(const Mutexff&) = delete;
	Mutexff& operator=(const Mutexff&) = delete;

	void lock();
	void unlock();
	pthread_mutex_t* getPthreadMutex(){
		return &mutex_;
	}
private:
	pthread_mutex_t mutex_;

	void checkError(int);
};//Mutex

}//namespace firey

#endif//MUTEX_H_
