#include "dbus_server.h"

DBusServer::DBusServer()
: logger(log4cxx::Logger::getLogger("DBusServer")),
  dispatcher(DBus::BusDispatcher()),
  started(false)
{

}

DBusServer::~DBusServer()
{
	LOG4CXX_DEBUG(logger, "Shutting down D-Bus server..");
	if (this->started)
		this->dispatcher.leave();
}

void DBusServer::execute()
{
	LOG4CXX_DEBUG(logger, "Initializing D-Bus server...");
	DBus::default_dispatcher = &this->dispatcher;
	DBus::Connection bus = DBus::Connection::SystemBus();
	bus.request_name(DOUANE_SERVER_NAME);

	this->douane = new Douane(bus);

	LOG4CXX_DEBUG(logger, "D-Bus server Initialized");

	LOG4CXX_DEBUG(logger, "Starting D-Bus server...");
	this->started = true;
	this->dispatcher.enter();
}

void DBusServer::new_network_activity(NetworkActivity * activity) const
{
	this->douane->fire_new_activity(activity);
}