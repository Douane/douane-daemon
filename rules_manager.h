#ifndef RULES_MANAGER_H
#define RULES_MANAGER_H

#include <boost/signals2.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <log4cxx/logger.h>
#include "rule.h"
#include "network_activity.h"
#include "tools.h"

/*
** RulesManager is a "rules engine" that is responsible to store, ask and synchronize rules
*  with the kernel module
*/
class RulesManager
{
  public:
    /*
    ** Signals
    */
    typedef boost::signals2::signal<void(const Rule*)>              signalNewRuleCreated;
    typedef signalNewRuleCreated::slot_type                         signalNewRuleCreatedType;
    typedef boost::signals2::signal<void(const Rule*)>              signalRuleDeleted;
    typedef signalRuleDeleted::slot_type                            signalRuleDeletedType;
    typedef boost::signals2::signal<void(const NetworkActivity*)>   signalNewUnknownActivity;
    typedef signalNewUnknownActivity::slot_type                     signalNewUnknownActivityType;

    /*
    ** Constructors and Destructor
    */
    RulesManager(void);
    virtual ~RulesManager(void);

    /*
    ** Instance methods
    */
    const std::map<std::string, const Rule>                         get_valid_rules(void) const;
    const Rule *                                                    search_valid_rule_for(const NetworkActivity * activity) const;
    void                                                            store_pending_rule_from_activity(const NetworkActivity * activity);
    void                                                            lookup_activity(const NetworkActivity * activity);
    void                                                            make_rule_from(const std::string& executable_sha256, const bool allow);
    int                                                             save_rules(void) const;
    int                                                             load_rules_from_file(void);
    void                                                            push_rules(void) const;
    bool                                                            delete_rule_for_sha256(const std::string &executable_sha256);

    /*
    ** Signals methods
    */
    static boost::signals2::connection                              on_new_rule_created_connect(const signalNewRuleCreatedType &slot);
    static boost::signals2::connection                              on_rule_deleted_connect(const signalRuleDeletedType &slot);
    static boost::signals2::connection                              on_new_unknown_activity_connect(const signalNewUnknownActivityType &slot);

  private:
    log4cxx::LoggerPtr                                              logger;
    std::map<std::string, const Rule>                               valid_rules;
    std::map<std::string, Rule>                                     pending_rules;

    void                                                            make_rule(const std::string executable_sha256, const std::string path, const bool allow);

    static signalNewRuleCreated                                     new_rule_created;
    static signalRuleDeleted                                        rule_deleted;
    static signalNewUnknownActivity                                 new_unknown_activity;
};

#endif
