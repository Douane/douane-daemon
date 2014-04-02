#ifndef DOUANE_H
#define DOUANE_H

#include <log4cxx/logger.h>
#include <boost/signals2.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include "interface/org_zedroot_douane_adaptor.h"
#include "../network_activity.h"
#include "../rules_manager.h"

/*
**  Communicate with the Kernel module.
*/
class Douane : public org::zedroot::Douane_adaptor,
               public DBus::IntrospectableAdaptor,
               public DBus::PropertiesAdaptor,
               public DBus::ObjectAdaptor
{
  public:
    /*
    ** Signals
    */
    typedef boost::signals2::signal<void(const std::string&, const bool)>   signalNewRuleReceived;
    typedef signalNewRuleReceived::slot_type                                signalNewRuleReceivedType;

    /*
    ** Constructors and Destructor
    */
    Douane(DBus::Connection &connection);
    virtual ~Douane(void);

    /*
    ** Instance methods
    */
    void                                                                    set_rules_manager(RulesManager * rules_manager);
    std::string                                                             RegisterAsDialogProcess(void);
    void                                                                    UnregisterDialogProcess(const std::string& process_id);
    virtual std::vector< ::DBus::Struct< std::string, std::string, bool > > GetRules(void);
    virtual bool                                                            DeleteRule(const std::string& rule_id);
    virtual void                                                            CreateRule(const std::string& rule_id, const bool& allow);
    void                                                                    emit_new_activity_to_be_validated_signal(const NetworkActivity * network_activity);

    /*
    ** Signals methods
    */
    static boost::signals2::connection                                      on_new_rule_received_connect(const signalNewRuleReceivedType &slot);

  private:
    log4cxx::LoggerPtr                                                      logger;
    RulesManager *                                                          rules_manager;

    static signalNewRuleReceived                                            new_rule_received;
};

#endif
