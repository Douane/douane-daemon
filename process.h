#ifndef DOUANE_PROCESS_H
#define DOUANE_PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <log4cxx/logger.h>
#include <openssl/sha.h>
#include <boost/lexical_cast.hpp> // boost::lexical_cast
#include "freedesktop/desktop_files.h"
#include "freedesktop/desktop_file.h"

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
		Process(int pid);
		Process(std::string &path);

		/**
		 *  Copy constructor
		 */
		Process(const Process &process);

		/**
		 *  Destructor.
		 */
		virtual ~Process();

		pid_t				process_pid;
		std::string			executable_sha256;
		std::string			path;
		std::string			executable_name;
		std::string			icon_name;
		std::string			printable_name;

	private:
		log4cxx::LoggerPtr	logger;
		std::string			proc_link;

		void				build_from_pid(void);
		void				try_get_process_path(void);
};

#endif
