#ifndef DOUANE_H
#define DOUANE_H

#include <log4cxx/logger.h>
#include "org_zedroot_douane.h"
#include "network_activity.h"

#define DEAMON_STATUS_ENABLE "Enable"
#define DEAMON_STATUS_DISABLE "Disable"

/**
 *  Communicate with the Kernel module.
 */
class Douane :	public org::zedroot::Douane_adaptor,
				public DBus::IntrospectableAdaptor,
				public DBus::PropertiesAdaptor,
				public DBus::ObjectAdaptor
{
	public:
		/**
		 *  Construct a Douane.
		 */
		Douane(DBus::Connection &connection);

		/**
		 *  Close DBus server.
		 */
		virtual ~Douane();

		/**
		 *  Receive NetworkActivity from NetlinkListener instance
		 *  Convert it to a DBus message
		 *  Then fire the signal passing the message
		 */
		void														fire_new_activity(NetworkActivity * activity);

		/**
		 *  Methods
		 */

		/**
		 * Enable / Disable the Douane
		 */
		virtual void												SetStatus(const bool& state);

		/**
		 *  Return the list of sotred firewall rules
		 */
		virtual std::vector< ::DBus::Struct< std::string, bool > >	GetRules();

	private:
		log4cxx::LoggerPtr											logger;
};

#endif