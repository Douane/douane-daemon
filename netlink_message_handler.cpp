#include "netlink_message_handler.h"

NetlinkMessageHandler::NetlinkMessageHandler(NetworkActivity * network_activity)
: logger(log4cxx::Logger::getLogger("NetlinkMessageHandler")),
  network_activity(network_activity)
{
  LOG4CXX_DEBUG(logger, this->network_activity->str());
}

NetlinkMessageHandler::~NetlinkMessageHandler(void)
{

}

void NetlinkMessageHandler::execute(void)
{
  // Broadcast the NetworkActivity (for D-Bus for example)
  this->new_network_activity(this->network_activity);

  // this->network_activity->request_resolve_remote_hostname();

  delete this;
}

/*
** Signals methods
*/
boost::signals2::connection NetlinkMessageHandler::on_new_network_activity_connect(const signalNewNetworkActivityType &slot)
{
  return new_network_activity.connect(slot);
}

NetlinkMessageHandler::signalNewNetworkActivity NetlinkMessageHandler::new_network_activity;
