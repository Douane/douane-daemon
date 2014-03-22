#include "dbus_server.h"

DBusServer::DBusServer(void)
: logger(log4cxx::Logger::getLogger("DBusServer")),
  dispatcher(DBus::BusDispatcher()),
  started(false)
{
  LOG4CXX_DEBUG(logger, "Initializing D-Bus server");
}

DBusServer::~DBusServer(void)
{
  LOG4CXX_DEBUG(logger, "Shutting down D-Bus server..");
  if (this->started)
    this->dispatcher.leave();
}

void DBusServer::set_rules_manager(RulesManager * rules_manager)
{
  this->rules_manager = rules_manager;
}

void DBusServer::execute()
{
  LOG4CXX_DEBUG(logger, "Initializing D-Bus server...");
  DBus::_init_threading();
  DBus::default_dispatcher = &this->dispatcher;
  DBus::Connection bus = DBus::Connection::SystemBus();
  bus.request_name(DOUANE_SERVER_NAME);

  this->douane = new Douane(bus);
  this->douane->set_rules_manager(this->rules_manager);

  LOG4CXX_DEBUG(logger, "D-Bus server Initialized");

  LOG4CXX_DEBUG(logger, "Starting D-Bus server...");
  this->started = true;
  this->dispatcher.enter();
}

void DBusServer::new_network_activity(NetworkActivity *) const
{
  // TODO: Implement a signal in the D-Bus server to publish network activities.
}

void DBusServer::signal_new_unknown_activity(const NetworkActivity * network_activity) const
{
  LOG4CXX_DEBUG(logger, "DBusServer::signal_new_unknown_activity...");

  this->douane->emit_new_activity_to_be_validated_signal(network_activity);
}
