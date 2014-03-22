#ifndef DBUS_SERVER_H
#define DBUS_SERVER_H

#include <log4cxx/logger.h>
#include <dbus-c++/dbus.h>
#include "douane.h"
#include "../thread.h"
#include "../network_activity.h"

#define DOUANE_SERVER_NAME "org.zedroot.Douane"

/*
**  Expose methods to other applications through a D-Bus server.
*/
class DBusServer : public Thread
{
  public:
    /*
    ** Constructors and Destructor
    */
    DBusServer(void);
    virtual ~DBusServer(void);

    /*
    ** Instance methods
    */
    void                set_rules_manager(RulesManager * rules_manager);
    void                execute(void);
    void                new_network_activity(NetworkActivity *) const;
    void                signal_new_unknown_activity(const NetworkActivity *) const;

  private:
    log4cxx::LoggerPtr  logger;
    DBus::BusDispatcher dispatcher;
    Douane *            douane;
    bool                started;
    RulesManager *      rules_manager;
};

#endif
