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

}

void Douane::set_rules_manager(RulesManager * rules_manager)
{
  this->rules_manager = rules_manager;
}

std::vector< ::DBus::Struct< std::string, std::string, bool > > Douane::GetRules()
{
  const std::map<std::string, const Rule> valid_rules = this->rules_manager->get_valid_rules();

  std::vector< ::DBus::Struct< std::string, std::string, bool > > rules;
  for(std::map<std::string, const Rule>::const_iterator it = valid_rules.begin(); it != valid_rules.end(); ++it)
  {
    ::DBus::Struct< std::string, std::string, bool > rule;
    rule._1 = it->second.executable_sha256;
    rule._2 = it->second.process_path;
    rule._3 = it->second.is_allowed();

    rules.push_back(rule);
  }

  return rules;
}

bool Douane::DeleteRule(const std::string& rule_id)
{
  LOG4CXX_DEBUG(logger, "Douane::DeleteRule(" << rule_id << ")");
  return this->rules_manager->delete_rule_for_sha256(rule_id);
}
