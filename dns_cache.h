#ifndef DNS_CACHE_H
#define DNS_CACHE_H

#include <boost/signals2.hpp>
#include <log4cxx/logger.h>
#include <algorithm>
#include "ip_address_translator.h"
#include "thread_lock.h"

class DnsCache
{
	public:
		typedef boost::signals2::signal<void(std::string, std::string)>	signalResolvedAddressFound;
		typedef signalResolvedAddressFound::slot_type					signalResolvedAddressFoundType;
		typedef boost::signals2::signal<void(std::string)>				signalResolvedAddressNotFound;
		typedef signalResolvedAddressNotFound::slot_type				signalResolvedAddressNotFoundType;

		DnsCache();
		virtual ~DnsCache();

		void									print_cache(void) const;
		void									lookup_from_cache(const std::string &ip_address);
		void									update_cache(const std::string &ip_address, const std::string &hostname);

		static	boost::signals2::connection		on_resolved_address_found_connect(const signalResolvedAddressFoundType &slot);
		static	boost::signals2::connection		on_resolved_address_not_found_connect(const signalResolvedAddressNotFoundType &slot);

	private:
		const std::string						find_from_resolved_addresses_cache(std::string &ip_address) const;
		bool									find_from_resolving_addresses_cache(std::string &ip_address) const;

		std::map<std::string, std::string>		resolved_addresses_cache;
		std::vector<std::string>				resolving_addresses_cache;

		log4cxx::LoggerPtr						logger;
		pthread_mutex_t							mutex;

		static signalResolvedAddressFound		resolved_address_found;
		static signalResolvedAddressNotFound	resolved_address_not_found;
};

#endif