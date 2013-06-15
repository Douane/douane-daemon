#ifndef DNS_CLIENT_H
#define DNS_CLIENT_H

#include <boost/signals2.hpp>
#include <log4cxx/logger.h>
#include "ip_address_translator.h"
#include "dns_cache.h"

class DnsClient
{
  public:
    /*
    ** Signals
    */
    typedef boost::signals2::signal<void(std::string)>              signalLookingUpForNewAddress;
    typedef signalLookingUpForNewAddress::slot_type                 signalLookingUpForNewAddressType;
    typedef boost::signals2::signal<void(std::string, std::string)> signalResultReceived;
    typedef signalResultReceived::slot_type                         signalResultReceivedType;
    typedef boost::signals2::signal<void(std::string, std::string)> signalIpAddressResolutionDone;
    typedef signalIpAddressResolutionDone::slot_type                signalIpAddressResolutionDoneType;

    /*
    ** Constructors and Destructor
    */
    DnsClient(const std::string &ip_address);
    virtual ~DnsClient(void);

    /*
    ** Instance methods
    */
    void                                  resolved_address_found(const std::string requested_address, const std::string resolved_address);
    void                                  resolved_address_not_found(const std::string requested_address);
    void                                  find_or_resolve_by_ip_address(void);
    // Signals methods
    static  boost::signals2::connection   on_looking_up_for_new_address_connect(const signalLookingUpForNewAddressType &slot);
    static  boost::signals2::connection   on_result_received_connect(const signalResultReceivedType &slot);
    static  boost::signals2::connection   on_ip_address_resolution_done_connect(const signalIpAddressResolutionDoneType &slot);

  private:
    void                                  resolve_ip_address(void);
    void                                  clear_signals(void);

    log4cxx::LoggerPtr                    logger;
    const std::string                     resolving_ip_address;
    boost::signals2::connection           resolved_address_found_signal;
    boost::signals2::connection           resolved_address_not_found_signal;
    bool                                  destroying;
    bool                                  already_disconnected;

    static signalLookingUpForNewAddress   looking_up_for_new_address;
    static signalResultReceived           result_received;
    static signalIpAddressResolutionDone  ip_address_resolution_done;
};

#endif
