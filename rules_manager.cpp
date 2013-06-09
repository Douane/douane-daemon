#include "rules_manager.h"

RulesManager::RulesManager()
: logger(log4cxx::Logger::getLogger("RulesManager"))
{
	this->load_rules_from_file();
	LOG4CXX_DEBUG(logger, "RulesManager initialization done");
}

RulesManager::~RulesManager()
{

}

const std::map<std::string, const Rule> RulesManager::get_valid_rules(void) const
{
	return this->valid_rules;
}

const Rule * RulesManager::search_valid_rule_for(const NetworkActivity * activity) const
{
	if (activity->process_has_been_detected())
	{
		std::map<std::string, const Rule>::const_iterator it = this->valid_rules.find(activity->process->get_executable_sha256());
		if (it == this->valid_rules.end())
		{
			LOG4CXX_DEBUG(logger, "No rule found for " << activity->process->path);
			return NULL;
		} else {
			LOG4CXX_DEBUG(logger, "A" << (it->second.is_allowed() ? "n allowing" : " disallowing") << " rule found for " << activity->process->path);
			return &it->second;
		}
	} else {
		return NULL;
	}
}

void RulesManager::lookup_activity(const NetworkActivity * activity)
{
	const Rule * rule = this->search_valid_rule_for(activity);
	if (rule == NULL)
	{
		if (activity->process_has_been_detected())
		{
			this->new_unknown_activity(activity);
		}
	}
}

void RulesManager::make_rule(const std::string executable_sha256, const std::string path, const bool allow)
{
	// Save the rule in memory
	this->valid_rules.insert(std::make_pair(executable_sha256, Rule(executable_sha256, path, allow)));

	// Broadcast the created rule
	this->new_rule_created(&this->valid_rules.find(executable_sha256)->second);
}

void RulesManager::make_rule_from(const NetworkActivity * activity, bool allow)
{
	this->make_rule(activity->process->get_executable_sha256(), activity->process->path, allow);

	// Write rules on disk
	this->save_rules();
}

boost::signals2::connection RulesManager::on_new_rule_created_connect(const signalNewRuleCreatedType &slot)
{
	return RulesManager::new_rule_created.connect(slot);
}
RulesManager::signalNewRuleCreated RulesManager::new_rule_created;

boost::signals2::connection RulesManager::on_rule_deleted_connect(const signalRuleDeletedType &slot)
{
	return RulesManager::rule_deleted.connect(slot);
}
RulesManager::signalRuleDeleted RulesManager::rule_deleted;

boost::signals2::connection RulesManager::on_new_unknown_activity_connect(const signalNewUnknownActivityType &slot)
{
	return RulesManager::new_unknown_activity.connect(slot);
}
RulesManager::signalNewUnknownActivity RulesManager::new_unknown_activity;

int RulesManager::save_rules(void) const
{
	// Build a JSON of each rules
	boost::property_tree::ptree root;
	for(std::map<std::string, const Rule>::const_iterator it = this->valid_rules.begin(); it != this->valid_rules.end(); ++it)
	{
		root.put_child(it->first, it->second.to_json());
	}

	boost::filesystem::path root_path = this->root_path();
	root_path /= "rules";
	std::ofstream rules_file;
	rules_file.open(root_path.string().c_str());
	if(!rules_file.is_open())
	{
		LOG4CXX_ERROR(logger, "Unable to open the file " << root_path.string());
		return -1;
	}

	boost::property_tree::write_json(rules_file, root);
	rules_file.close();

	// Only root user can access that file
	chmod(root_path.string().c_str(), 0600);

	return 0;
}

int RulesManager::load_rules_from_file(void)
{
	LOG4CXX_DEBUG(logger, "RulesManager::load_rules_from_file...");

	boost::filesystem::path root_path = this->root_path();
	root_path /= "rules";
	std::ifstream rules_file;
	rules_file.open(root_path.string().c_str());
	if(!rules_file.is_open())
	{
		LOG4CXX_ERROR(logger, "Unable to open the file " << root_path.string());
		return -1;
	}

	boost::property_tree::ptree root;
	try
	{
		boost::property_tree::json_parser::read_json(rules_file, root);

		// For each JSON node create a Rule instance
		BOOST_FOREACH(boost::property_tree::ptree::value_type &node, root)
		{
			this->make_rule(node.first.data(), node.second.get<std::string>("path"), node.second.get<std::string>("allowed") == "true");
		}
	} catch(boost::exception & e)
	{
		LOG4CXX_ERROR(logger, "Unable to load rules: " << boost::diagnostic_information(e));
	}

	rules_file.close();

	LOG4CXX_INFO(logger, "Loaded " << this->valid_rules.size() << " rules");

	return 0;
}

boost::filesystem::path RulesManager::root_path(void) const
{
	boost::filesystem::path root_path("/opt/douane");

	// Ensure folder exists
	mkdir(root_path.string().c_str(), 0700);

	return root_path;
}

void RulesManager::push_rules(void) const
{
	for(std::map<std::string, const Rule>::const_iterator it = this->valid_rules.begin(); it != this->valid_rules.end(); ++it)
	{
		this->new_rule_created(&it->second);
	}
}

bool RulesManager::delete_rule_for_sha256(const std::string &executable_sha256)
{
	// Search the Rule instance for the given executable_sha256
	std::map<std::string, const Rule>::iterator it = this->valid_rules.find(executable_sha256);
	if (it == this->valid_rules.end())
	{
		// When not found return false so that the client is aware
		return false;
	} else {
		/**
		 *  As it could be many rules for the same application (but different versions)
		 *  the LKM is filering only the active/installed/running version of the application.
		 *
		 *  Until we aren't deleting the active/installed/running version rule we don't have to
		 *  emit the rule_deleted signal.
		 *  But when removing the active/installed/running version rule then we have to emit the signal.
		 */
		// Compare passed executable_sha256 with active/installed/running version executable SHA256
		Tools tools;
		const std::string running_executable_sha256 = tools.make_sha256_from(it->second.process_path);

		if (executable_sha256 == running_executable_sha256)
		{
			LOG4CXX_DEBUG(logger, "Deleting rule with executable SHA256 " << executable_sha256);
			this->rule_deleted(&it->second);
		}

		// In any cases remove the delete rule from the valid_rules std::map
		this->valid_rules.erase(it);

		// Save it to the disk
		this->save_rules();

		return true;
	}
}