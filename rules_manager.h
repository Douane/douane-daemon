#ifndef RULES_MANAGER_H
#define RULES_MANAGER_H

#include <boost/signals2.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <log4cxx/logger.h>
#include "rule.h"
#include "network_activity.h"

class RulesManager
{
	public:
		typedef boost::signals2::signal<void(const Rule*)>				signalNewRuleCreated;
		typedef signalNewRuleCreated::slot_type							signalNewRuleCreatedType;
		typedef boost::signals2::signal<void(const NetworkActivity *)>	signalNewUnknownActivity;
		typedef signalNewUnknownActivity::slot_type						signalNewUnknownActivityType;

		RulesManager();

		virtual ~RulesManager();

		const std::map<std::string, const Rule>	get_valid_rules(void) const;
		const Rule *							search_valid_rule_for(const NetworkActivity * activity) const;
		void									lookup_activity(const NetworkActivity * activity);
		void									make_rule_from(const NetworkActivity * activity, bool allow);
		static boost::signals2::connection		on_new_rule_created_connect(const signalNewRuleCreatedType &slot);
		static boost::signals2::connection		on_new_unknown_activity_connect(const signalNewUnknownActivityType &slot);
		int										save_rules(void) const;
		int										load_rules_from_file(void);
		void									push_rules(void) const;
		bool									delete_rule_for_sha256(const std::string &executable_sha256);

	private:
		void								make_rule(const std::string executable_sha256, const std::string path, const bool allow);
		boost::filesystem::path				root_path(void) const;

		log4cxx::LoggerPtr					logger;
		std::map<std::string, const Rule>	valid_rules;
		std::map<std::string, Rule>			pending_rules;

		static signalNewRuleCreated			new_rule_created;
		static signalNewUnknownActivity		new_unknown_activity;
};

#endif
