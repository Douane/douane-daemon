#include "douane.h"

Douane::Douane(DBus::Connection &connection)
: DBus::ObjectAdaptor(connection, "/org/zedroot/Douane"),
  logger(log4cxx::Logger::getLogger("Douane"))
{
  LOG4CXX_DEBUG(logger, "Douane::Douane...");
  DaemonVersion = "1.0.0";
}

Douane::~Douane(void)
{
  LOG4CXX_DEBUG(logger, "Douane::~Douane...");
}

void Douane::set_rules_manager(RulesManager * rules_manager)
{
  LOG4CXX_DEBUG(logger, "Douane::set_rules_manager...");
  this->rules_manager = rules_manager;
  LOG4CXX_DEBUG(logger, "Douane::set_rules_manager done");
}

std::vector< ::DBus::Struct< std::string, std::string, bool > > Douane::GetRules()
{
  LOG4CXX_DEBUG(logger, "Douane::GetRules | this->rules_manager->get_valid_rules()...");
  const std::map<std::string, const Rule> valid_rules = this->rules_manager->get_valid_rules();
  LOG4CXX_DEBUG(logger, "Douane::GetRules | this->rules_manager->get_valid_rules() Done");

  std::vector< ::DBus::Struct< std::string, std::string, bool > > rules;
  for(std::map<std::string, const Rule>::const_iterator it = valid_rules.begin(); it != valid_rules.end(); ++it)
  {
    ::DBus::Struct< std::string, std::string, bool > rule;
    rule._1 = it->second.executable_sha256;
    rule._2 = it->second.process_path;
    rule._3 = it->second.is_allowed();

    LOG4CXX_DEBUG(logger, "Douane::GetRules | rules.push_back()...");
    rules.push_back(rule);
    LOG4CXX_DEBUG(logger, "Douane::GetRules | rules.push_back() Done");
  }

  return rules;
}

bool Douane::DeleteRule(const std::string& rule_id)
{
  LOG4CXX_DEBUG(logger, "Douane::DeleteRule(" << rule_id << ")");
  return this->rules_manager->delete_rule_for_sha256(rule_id);
}

void Douane::CreateRule(const std::string& rule_id, const bool& allowed)
{
  this->new_rule_received(rule_id, allowed);
}

void Douane::emit_new_activity_to_be_validated_signal(const NetworkActivity * network_activity)
{
  LOG4CXX_DEBUG(logger, "Douane::emit_new_activity_to_be_validated_signal...");

  ::DBus::Struct< std::string, std::string, std::string, std::string > activity;
  activity._1 = network_activity->process->get_executable_sha256();
  activity._2 = network_activity->process->printable_name;
  activity._3 = network_activity->process->executable_name;
  activity._4 = network_activity->process->icon_name;

  LOG4CXX_DEBUG(logger, "Firing D-Bus signal NewActivityToBeValidated...");
  this->NewActivityToBeValidated(activity);
}

/*
** Signals methods
*/
boost::signals2::connection Douane::on_new_rule_received_connect(const signalNewRuleReceivedType &slot)
{
  return new_rule_received.connect(slot);
}

Douane::signalNewRuleReceived Douane::new_rule_received;
