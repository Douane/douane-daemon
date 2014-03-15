#ifndef DOUANE_NETLINK_LISTENER_H
#define DOUANE_NETLINK_LISTENER_H

#include <errno.h>
#include <boost/signals2.hpp>
#include <log4cxx/logger.h>
#include "network_activity.h"
#include "netlink_socket.h"
#include "rules_manager.h"
#include "netlink_message_handler.h"

/*
**  Receive network activities from the Kernel module.
*/
class NetlinkListener
: public NetlinkSocket
{
  public:
    /*
    ** Signals
    */
    typedef boost::signals2::signal<void()>           signalConnectedToKernelModule;
    typedef signalConnectedToKernelModule::slot_type  signalConnectedToKernelModuleType;

    /*
    ** Constructors and Destructor
    */
    NetlinkListener(void);
    virtual ~NetlinkListener(void);

    /*
    ** Instance methods
    */
    void                                  set_processes_manager(ProcessesManager * processes_manager);
    void                                  start(void);
    void                                  die(void);
    void                                  send_rule(const Rule * rule);
    void                                  delete_rule(const Rule * rule);
    void                                  say_goodbye(void);

    /*
    ** Signals methods
    */
    static boost::signals2::connection    on_connected_to_kernel_module_connect(const signalConnectedToKernelModuleType &slot);

  private:
    log4cxx::LoggerPtr                    logger;
    ProcessesManager *                    processes_manager;
    bool                                  running;

    void                                  send_hand_check(void);
    struct network_activity *             build_message(int kind);
    void                                  send_message(struct network_activity * activity);

    static signalConnectedToKernelModule  connected_to_kernel_module;
};

#endif
