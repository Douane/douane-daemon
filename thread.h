#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Thread
{
  public:
    /*
    ** Instance methods
    */
    void          start(void);
    void          start_and_detach(void);
    virtual void  execute(void);
    void          wait_for_exit(void);

  private:
    void          create(bool detach);
    pthread_t     handle;
};

#endif
