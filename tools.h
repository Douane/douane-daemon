#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <stdio.h>
#include <string.h>			// strerror()
#include <errno.h>			// errno
#include <fstream>			// std::ifstream
#include <iomanip>			// setw(), setfill()
#include <openssl/sha.h>
#include <log4cxx/logger.h>
#include <proc/readproc.h>	// openproc(), readproc()

class Tools
{
	public:
		Tools();

		virtual ~Tools();

		const std::string	make_sha256_from(const std::string &path) const;

		pid_t				process_pid_from_process_name(const std::string &path) const;

	private:
		const std::string	executable_to_string(const std::string &path) const;

		log4cxx::LoggerPtr	logger;
};

#endif
