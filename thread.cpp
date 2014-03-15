#include "thread.h"
#include <iostream>

extern "C"
{
  // this C function will be used to receive the thread and pass it back to the Thread instance
  void* thread_catch(void* arg)
  {
    Thread* t = static_cast<Thread*>(arg);
    t->execute();
    return 0;
  }
}

// method which starts the new thread
void Thread::start(void)
{
  Thread::create(false);
}

// method which starts the new thread
void Thread::start_and_detach(void)
{
  Thread::create(true);
}

// code which will be run on the new thread
void Thread::execute(void)
{
  std::cout << "You must implement an execute() method!" << std::endl;
}

// wait until this thread has finished executing
void Thread::wait_for_exit(void)
{
  pthread_join(handle, NULL);
}

/*
** Private
*/

// method which starts the new thread
void Thread::create(bool detach)
{
  pthread_create(&handle, 0, thread_catch, this);
  if (detach)
  {
    pthread_detach(handle);
  }
}
