#ifndef DOUANE_NETLINK_SOCKET_H
#define DOUANE_NETLINK_SOCKET_H

#include "socket.h"

/**
 *  Communicate with the Kernel module.
 */
class NetlinkSocket : public Socket
{
	public:
		/**
		 *  Construct a NetlinkSocket and initialize Netlink socket.
		 *  @exception SocketException thrown if socket initialization failed.
		 */
		NetlinkSocket();

		/**
		 *  Ensure Netlink socket is closed.
		 */
		virtual ~NetlinkSocket();

	private:
		log4cxx::LoggerPtr logger;
};

#endif
