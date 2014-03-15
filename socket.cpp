#include "socket.h"

Socket::Socket(int domain, int type, int protocol)
: logger(log4cxx::Logger::getLogger("Socket"))
{
  this->domain    = domain;
  this->type      = type;
  this->protocol  = protocol;
}

Socket::Socket(const Socket &socket)
: logger(log4cxx::Logger::getLogger("Socket"))
{
  this->sock_fd             = socket.sock_fd;
  this->source_address      = socket.source_address;
  this->destination_address = socket.destination_address;
  this->destination_address = socket.destination_address;
  this->domain              = socket.domain;
  this->type                = socket.type;
  this->protocol            = socket.protocol;
}

Socket::~Socket(void)
{
  if (this->sock_fd > 0)
  {
    LOG4CXX_DEBUG(logger, "Closing socket " << this->sock_fd);
    if (close(this->sock_fd) == 0)
    {
      LOG4CXX_DEBUG(logger, "Socket closed");
      this->sock_fd = 0;
    } else {
      LOG4CXX_WARN(logger, "Unable to close the Socket!");
    }
  } else {
    LOG4CXX_DEBUG(logger, "No socket to be closed");
  }
}

void Socket::create_and_bind(void)
{
  if ((this->sock_fd = socket(domain, type, protocol)) < 0)
  {
    LOG4CXX_ERROR(logger, "Unable to create a new socket: " << strerror(errno));
    throw SocketException("Unable to create a new socket.");
  }

  if (bind(this->sock_fd, (struct sockaddr*)&source_address, sizeof(struct sockaddr_nl)) == -1)
  {
    LOG4CXX_ERROR(logger, "Unable to bind socket: " << strerror(errno));
    throw SocketException("Unable to bind socket.");
  }
}
