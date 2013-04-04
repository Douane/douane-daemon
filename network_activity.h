#ifndef DOUANE_NETWORK_ACTIVITY_H
#define DOUANE_NETWORK_ACTIVITY_H

#include <boost/signals2.hpp>
#include <cstring>
#include <netinet/in.h> /* IPPROTO_* */
#include <log4cxx/logger.h>
#include "network_activity_message.h"
#include "process.h"
#include "processes_manager.h"
#include "dns_client.h"

/**
 *  A received network activity from Kernel.
 */
class NetworkActivity
{
	public:
		typedef boost::signals2::signal<void(const std::string, const std::string)>	signalHostnameUpdated;
		typedef signalHostnameUpdated::slot_type									signalHostnameUpdatedType;

		/**
		 *  Construct a NetworkActivity.
		 */
		NetworkActivity(struct network_activity * activity, ProcessesManager * processes_manager);

		/**
		 *  Copy constructor
		 */
		NetworkActivity(const NetworkActivity &activity);

		/**
		 *  Destructor.
		 */
		virtual ~NetworkActivity();

		/**
		 *  Return a std::string describing the current network activity
		 */
		const std::string					str(void) const;

		/**
		 *  Return true if the network activity type is Incoming
		 */
		bool								incoming(void) const;

		/**
		 *  Return true if the network activity type is Outcoming
		 */
		bool								outcoming(void) const;

		const std::string					append_port_code(void) const;

		void								request_resolve_remote_hostname(void);

		void								update_hostname(const std::string requested_address, const std::string resolved_hostname);

		const std::string					hostname_or_ip_destination(void) const;

		static boost::signals2::connection	on_hostname_updated_connect(const signalHostnameUpdatedType &slot);

		std::string							devise_name;
		std::string							protocol;
		const Process *						process;
		std::string							ip_source;
		int									port_source;
		std::string							ip_destination;
		int									port_destination;
		int									size;
		int									process_id;
		std::string							hostname;

		bool								process_has_been_detected(void) const { return this->process != NULL; };

	private:
		log4cxx::LoggerPtr					logger;
		ProcessesManager *					processes_manager;
		boost::signals2::connection			update_signal;
		int									in_or_out;
		DnsClient *							dns_client;

		std::string							protocol_name_from_id(int id) const;

		static signalHostnameUpdated		hostname_updated;
};

#endif
