#ifndef PROCESSES_MANAGER_H
#define PROCESSES_MANAGER_H

#include <log4cxx/logger.h>
#include "process.h"

class ProcessesManager
{
	public:
		ProcessesManager();

		virtual ~ProcessesManager();

		void							set_desktop_files(const DesktopFiles * desktop_files);

		const Process *					find_or_create_from_pid(pid_t process_id);

	private:
		Process *						make_new_process(std::string &path, pid_t process_id);
		const std::string				executable_to_string(std::string &path) const;
		const std::string				make_sha256_from(std::string &path) const;

		log4cxx::LoggerPtr				logger;
		const DesktopFiles *			desktop_files;
		std::map<std::string, Process>	processes;
};

#endif
