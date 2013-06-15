#ifndef DOUANE_SOCKET_H
#define DOUANE_SOCKET_H

#include <cstring>
#include <iostream>
#include <errno.h>
#include <log4cxx/logger.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

/*
**  Error classes used to be thrown
*/
class SocketException : public std::runtime_error
{
  public:
    SocketException(std::string message) : std::runtime_error(message) { }
};

/*
**  Communicate with the Kernel module.
*/
class Socket
{
  public:

    /*
    ** Constructors and Destructor
    */
    Socket(int domain, int type, int protocol);
    Socket(const Socket &socket);
    virtual ~Socket(void);

    /*
    ** Instance methods
    */
    void                create_and_bind(void);

  protected:
    int                 sock_fd;
    struct sockaddr_nl  source_address;
    struct sockaddr_nl  destination_address;

  private:
    log4cxx::LoggerPtr  logger;
    int                 domain;
    int                 type;
    int                 protocol;
};

#endif
