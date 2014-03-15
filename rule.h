#ifndef RULES_H
#define RULES_H

#include <log4cxx/logger.h>
#include <boost/property_tree/json_parser.hpp>
#include <libgen.h>
#include "tools.h"

class Rule
{
  public:
    /*
    ** Constructors and Destructor
    */
    Rule(const std::string executable_sha256, const std::string process_path, const bool allow);
    virtual ~Rule(void);

    /*
    ** Instance methods
    */
    bool                        is_allowed(void) const;
    boost::property_tree::ptree to_json(void) const;

    const std::string           executable_sha256;
    const std::string           process_path;
    const std::string           process_name;
    const bool                  allow;

  private:
    log4cxx::LoggerPtr          logger;

    std::string                 update_process_name_from_path(void) const;
};

#endif
