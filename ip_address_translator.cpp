#include "ip_address_translator.h"

IpAddressTranslator::IpAddressTranslator(void)
: logger(log4cxx::Logger::getLogger("IpAddressTranslator"))
{

}

IpAddressTranslator::~IpAddressTranslator(void)
{

}

const std::string IpAddressTranslator::translate(const std::string &ip_address)
{
  return IpAddressTranslator::resolver(ip_address);
}

/*
** Private
*/

const std::string IpAddressTranslator::resolver(const std::string &ip_address)
{
  struct in_addr ipv4addr;
  inet_pton(AF_INET, ip_address.c_str(), &ipv4addr);
  struct hostent * he = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);

  return he == NULL ? "" : he->h_name;
}
