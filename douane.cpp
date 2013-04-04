#include "douane.h"

Douane::Douane(DBus::Connection &connection)
: DBus::ObjectAdaptor(connection, "/org/zedroot/Douane"),
  logger(log4cxx::Logger::getLogger("Douane"))
{
	LOG4CXX_DEBUG(logger, "Douane::Douane...");
	DaemonVersion = "1.0.0";
	DaemonStatus = DEAMON_STATUS_DISABLE;
}

Douane::~Douane()
{

}

void Douane::fire_new_activity(NetworkActivity * activity)
{
	::DBus::Struct< std::string, std::string, std::string, int32_t, std::string, int32_t, int32_t > network_activity;
	network_activity._1 = activity->devise_name;
	network_activity._2 = activity->protocol;
	network_activity._3 = activity->ip_source;
	network_activity._4 = activity->port_source;
	network_activity._5 = activity->ip_destination;
	network_activity._6 = activity->port_destination;
	network_activity._7 = activity->size;

	this->NewIncomingActivity(network_activity);
}

void Douane::SetStatus(const bool& state)
{
	if (state)
		DaemonStatus = DEAMON_STATUS_ENABLE;
	else
		DaemonStatus = DEAMON_STATUS_DISABLE;
}

std::vector< ::DBus::Struct< std::string, bool > > Douane::GetRules()
{
	LOG4CXX_DEBUG(logger, "Douane::GetRules...");

	// ::DBus::Struct< std::string, bool > rule;
	// rule._1 = "Test";
	// rule._2 = true;

	// this->NewIncomingActivity(rule);

	std::vector< ::DBus::Struct< std::string, bool > > rules;
	// rules.push_back(rule);
	return rules;
}