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

/*
**  A received network activity from Kernel.
*/
class NetworkActivity
{
  public:
    /*
    ** Signals
    */
    typedef boost::signals2::signal<void(const std::string, const std::string)> signalHostnameUpdated;
    typedef signalHostnameUpdated::slot_type                                    signalHostnameUpdatedType;

    /*
    ** Constructors and Destructor
    */
    NetworkActivity(struct network_activity * activity, ProcessesManager * processes_manager);
    NetworkActivity(const NetworkActivity &activity);
    virtual ~NetworkActivity(void);

    /*
    ** Instance methods
    */
    const std::string                   str(void) const;
    bool                                incoming(void) const;
    bool                                outcoming(void) const;
    const std::string                   append_port_code(void) const;
    void                                request_resolve_remote_hostname(void);
    void                                update_hostname(const std::string requested_address, const std::string resolved_hostname);
    const std::string                   hostname_or_ip_destination(void) const;
    static boost::signals2::connection  on_hostname_updated_connect(const signalHostnameUpdatedType &slot);
    bool                                process_has_been_detected(void) const;

    /*
    ** Attributes
    */
    std::string                         devise_name;
    std::string                         protocol;
    const Process *                     process;
    std::string                         ip_source;
    int                                 port_source;
    std::string                         ip_destination;
    int                                 port_destination;
    int                                 size;
    std::string                         hostname;

  private:
    log4cxx::LoggerPtr                  logger;
    ProcessesManager *                  processes_manager;
    boost::signals2::connection         update_signal;
    DnsClient *                         dns_client;

    std::string                         protocol_name_from_id(int id) const;

    static signalHostnameUpdated        hostname_updated;
};

#endif
