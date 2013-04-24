#ifndef DBUS_SERVER_H
#define DBUS_SERVER_H

#include <log4cxx/logger.h>
#include <dbus-c++/dbus.h>
#include "douane.h"
#include "thread.h"
#include "network_activity.h"

#define DOUANE_SERVER_NAME "org.zedroot.Douane"

/**
 *  Expose methods to other applications through a D-Bus server.
 */
class DBusServer : public Thread
{
	public:
		/**
		 *  Constructor
		 */
		DBusServer(void);

		/**
		 *  Shutdown the D-Bus server.
		 */
		virtual ~DBusServer(void);

		void				set_rules_manager(RulesManager * rules_manager);

		/**
		 *  Start a D-Bus Server
		 */
		void				execute(void);

		/*
		 *  Forward NetworkActivity instance to the D-Bus object Douane
		 *  so that it can fire the signal NewActivity.
		 */
		void				new_network_activity(NetworkActivity * activity) const;

	private:
		log4cxx::LoggerPtr	logger;
		DBus::BusDispatcher	dispatcher;
		Douane *			douane;
		bool				started;
		RulesManager *		rules_manager;
};

#endif
