#include "netlink_socket.h"

NetlinkSocket::NetlinkSocket(void)
: Socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK),
  logger(log4cxx::Logger::getLogger("NetlinkSocket"))
{
  memset(&this->source_address, 0, sizeof(struct sockaddr_nl));
  this->source_address.nl_family  = AF_NETLINK;
  this->source_address.nl_pid     = getpid();   /* self pid */
  this->source_address.nl_groups  = 0;          /* unicast */

  memset(&this->destination_address, 0, sizeof(struct sockaddr_nl));
  this->destination_address.nl_family = AF_NETLINK;
  this->destination_address.nl_pid    = 0;      /* To the Linux Kernel */
  this->destination_address.nl_groups = 0;      /* unicast */

  // Socket::create_and_bind
  this->create_and_bind();
}

NetlinkSocket::~NetlinkSocket(void)
{

}
