#ifndef DOUANE_PROCESS_H
#define DOUANE_PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <sstream>
#include <string>
#include <iostream>
#include <log4cxx/logger.h>
#include <boost/lexical_cast.hpp> // boost::lexical_cast
#include <boost/filesystem/path.hpp> // boost::filesystem::path
#include <boost/filesystem/operations.hpp> // boost::filesystem::read_symlink
#include <sys/stat.h>
#include "tools.h"

/**
 *  Error classes used to be thrown
 */
class ProcessException : public std::runtime_error
{
	public:
		ProcessException(std::string message) : std::runtime_error(message) { }
};

/**
 *  Represent the process that generated the network activity.
 */
class Process
{
	public:
		/**
		 *  Construct a Process from its PID
		 */
		Process(const pid_t pid);

		/**
		 *  Copy constructor
		 */
		Process(const Process &process);

		/**
		 *  Destructor.
		 */
		virtual ~Process();

		const pid_t			process_pid;
		std::string			path;
		std::string			executable_name;
		std::string			icon_name;
		std::string			printable_name;
		const std::string	get_executable_sha256(void) const { return this->executable_sha256; };

	private:
		log4cxx::LoggerPtr	logger;
		std::string			proc_link;
		std::string			executable_sha256;

		void				build_from_pid(void);
		void				try_get_process_path(void);
		void				get_executable_name_from_path(void);
};

#endif
