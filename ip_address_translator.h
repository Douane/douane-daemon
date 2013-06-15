#ifndef IP_ADDRESS_TRANSLATOR_H
#define IP_ADDRESS_TRANSLATOR_H

#include <log4cxx/logger.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>

class IpAddressTranslator
{
  public:
    /*
    ** Constructors and Destructor
    */
    IpAddressTranslator(void);
    virtual ~IpAddressTranslator(void);

    /*
    ** Class methods
    */
    static const std::string  translate(const std::string &ip_address);

  private:
    static const std::string  resolver(const std::string &ip_address);
    log4cxx::LoggerPtr        logger;
};

#endif
