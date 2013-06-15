#ifndef DOUANE_NETLINK_SOCKET_H
#define DOUANE_NETLINK_SOCKET_H

#include "socket.h"

/*
**  Communicate with the Kernel module.
*/
class NetlinkSocket : public Socket
{
  public:

    /*
    ** Constructors and Destructor
    */
    NetlinkSocket(void);
    virtual ~NetlinkSocket(void);

  private:
    log4cxx::LoggerPtr  logger;
};

#endif
