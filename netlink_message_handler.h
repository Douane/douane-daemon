#ifndef NETLINK_MESSAGE_HANDLER_H
#define NETLINK_MESSAGE_HANDLER_H

#include <boost/signals2.hpp>
#include <log4cxx/logger.h>
#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include "network_activity.h"

class NetlinkMessageHandler : public Thread
{
  public:
    /*
    ** Signals
    */
    typedef boost::signals2::signal<void(NetworkActivity*)> signalNewNetworkActivity;
    typedef signalNewNetworkActivity::slot_type             signalNewNetworkActivityType;

    /*
    ** Constructors and Destructor
    */
    NetlinkMessageHandler(NetworkActivity * network_activity);
    virtual ~NetlinkMessageHandler(void);

    /*
    ** Instance methods
    */
    void                                execute(void);

    /*
    ** Signals methods
    */
    static boost::signals2::connection  on_new_network_activity_connect(const signalNewNetworkActivityType &slot);

  private:
    log4cxx::LoggerPtr                  logger;
    ProcessesManager *                  processes_manager;
    NetworkActivity *                   network_activity;
    struct network_activity *           activity_struct;

    static signalNewNetworkActivity     new_network_activity;
};

#endif
