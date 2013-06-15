#ifndef THREAD_LOCK_H
#define THREAD_LOCK_H

#include <pthread.h>

class ThreadLock {
  pthread_mutex_t &mutex;

  public:
    /*
    ** Constructors and Destructor
    */
    ThreadLock(pthread_mutex_t &m) :mutex(m)  { pthread_mutex_lock(&mutex); }
    ~ThreadLock(void)                         { pthread_mutex_unlock(&mutex); }
};

#endif
