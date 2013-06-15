#include "dns_cache.h"

DnsCache::DnsCache(void)
: logger(log4cxx::Logger::getLogger("DnsCache"))
{
  pthread_mutex_init(&mutex, NULL);
}

DnsCache::~DnsCache(void)
{
  pthread_mutex_destroy(&mutex);
}

void DnsCache::print_cache(void) const
{
  std::cout << "[DnsCache] " << this->resolved_addresses_cache.size() << " address(es)" << std::endl;
  for(std::map<std::string, std::string>::const_iterator it=this->resolved_addresses_cache.begin(); it!=this->resolved_addresses_cache.end(); ++it)
    std::cout << "[DnsCache] " << it->first << " -> \"" << it->second << "\"" << std::endl;
}

void DnsCache::lookup_from_cache(const std::string &ip_address)
{
  // this->print_cache();

  std::map<std::string, std::string>::const_iterator it = this->resolved_addresses_cache.find(ip_address);
  if (it == this->resolved_addresses_cache.end())
  {
    ThreadLock thread_lock(mutex);

    std::map<std::string, std::string>::const_iterator it2 = this->resolved_addresses_cache.find(ip_address);
    if (it2 == this->resolved_addresses_cache.end() && this->resolved_address_not_found.num_slots() > 0)
    {
      std::cout << "[DnsCache] Fireing " << this->resolved_address_not_found.num_slots() << " resolved_address_not_found(\"" << ip_address << "\")..." << std::endl;
      this->resolved_address_not_found(ip_address);
    } else {
      if (this->resolved_address_found.num_slots() > 0)
        this->resolved_address_found(ip_address, it2->second);
    }
  } else {
    if (this->resolved_address_found.num_slots() > 0)
      this->resolved_address_found(ip_address, it->second);
  }
}

void DnsCache::update_cache(const std::string &ip_address, const std::string &hostname)
{
  this->resolved_addresses_cache.insert(std::make_pair(ip_address, hostname));
}

boost::signals2::connection DnsCache::on_resolved_address_found_connect(const signalResolvedAddressFoundType &slot)
{
  return resolved_address_found.connect(slot);
}

DnsCache::signalResolvedAddressFound DnsCache::resolved_address_found;

boost::signals2::connection DnsCache::on_resolved_address_not_found_connect(const signalResolvedAddressNotFoundType &slot)
{
  return resolved_address_not_found.connect(slot);
}

DnsCache::signalResolvedAddressNotFound DnsCache::resolved_address_not_found;
