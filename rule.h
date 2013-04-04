#ifndef RULES_H
#define RULES_H

#include <log4cxx/logger.h>
#include <boost/property_tree/json_parser.hpp>
#include <libgen.h>

class Rule
{
	public:
		Rule(const std::string process_path, const bool allow);

		virtual ~Rule();

		bool						is_allowed() const;
		boost::property_tree::ptree	to_json() const;

		const std::string			process_path;
		const std::string			process_name;
		const bool					allow;

	private:
		std::string					update_process_name_from_path(void) const;
		log4cxx::LoggerPtr			logger;
};

#endif
