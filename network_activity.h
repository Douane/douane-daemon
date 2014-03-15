#ifndef DOUANE_NETWORK_ACTIVITY_H
#define DOUANE_NETWORK_ACTIVITY_H

#include <cstring>
#include <netinet/in.h> /* IPPROTO_* */
#include <log4cxx/logger.h>
#include "network_activity_message.h"
#include "process.h"
#include "processes_manager.h"

/*
**  A received network activity from Kernel.
*/
class NetworkActivity
{
  public:
    /*
    ** Constructors and Destructor
    */
    NetworkActivity(struct network_activity * activity, ProcessesManager * processes_manager);
    NetworkActivity(const NetworkActivity &activity);
    virtual ~NetworkActivity(void);

    /*
    ** Instance methods
    */
    bool                process_has_been_detected(void) const;
    const std::string   str(void) const;
    const std::string   append_port_code(void) const;

    /*
    ** Attributes
    */
    std::string         devise_name;
    std::string         protocol;
    const Process *     process;
    std::string         ip_source;
    int                 port_source;
    std::string         ip_destination;
    int                 port_destination;
    int                 size;

  private:
    log4cxx::LoggerPtr  logger;
    ProcessesManager *  processes_manager;

    std::string         protocol_name_from_id(int id) const;
};

#endif
