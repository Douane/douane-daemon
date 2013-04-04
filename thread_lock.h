#ifndef THREAD_LOCK_H
#define THREAD_LOCK_H

#include <pthread.h>

class ThreadLock {
	pthread_mutex_t &mutex;
public:
	ThreadLock(pthread_mutex_t &m) :mutex(m) { pthread_mutex_lock(&mutex); }
	~ThreadLock() { pthread_mutex_unlock(&mutex); }
};

#endif