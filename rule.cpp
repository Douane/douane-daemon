#include "rule.h"

Rule::Rule(const std::string executable_sha256, const std::string process_path, const bool allow)
: executable_sha256(executable_sha256),
  process_path(process_path),
  process_name(update_process_name_from_path()),
  allow(allow),
  logger(log4cxx::Logger::getLogger("Rule"))
{

}

Rule::~Rule(void)
{

}

bool Rule::is_allowed(void) const
{
  return this->allow;
}

boost::property_tree::ptree Rule::to_json(void) const
{
  boost::property_tree::ptree rule;
  rule.put("path", this->process_path);
  rule.put("allowed", this->is_allowed());
  return rule;
}

/*
** Private
*/

std::string Rule::update_process_name_from_path(void) const
{
  char path[PATH_MAX * 4] = "";
  std::copy(this->process_path.begin(), this->process_path.end(), path);
  return std::string(basename(path));
}
