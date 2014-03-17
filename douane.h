#ifndef DOUANE_H
#define DOUANE_H

#include <log4cxx/logger.h>
#include "org_zedroot_douane.h"
#include "network_activity.h"
#include "rules_manager.h"

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
    ** Constructors and Destructor
    */
    Douane(DBus::Connection &connection);
    virtual ~Douane(void);

    /*
    ** Instance methods
    */
    void                                                                    set_rules_manager(RulesManager * rules_manager);
    virtual std::vector< ::DBus::Struct< std::string, std::string, bool > > GetRules(void);
    virtual bool                                                            DeleteRule(const std::string& rule_id);

  private:
    log4cxx::LoggerPtr                                                      logger;
    RulesManager *                                                          rules_manager;
};

#endif
