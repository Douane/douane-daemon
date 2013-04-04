#ifndef DESKTOP_FILE_H
#define DESKTOP_FILE_H

#include <log4cxx/logger.h>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>
#include <fstream>

class DesktopFile
{
	public:
		DesktopFile(const boost::filesystem::path& path);
		virtual ~DesktopFile();

		boost::filesystem::path	path;

		const std::string					get_icon(void) const;
		const std::string					get_exec(void) const;
		const std::string					get_name(void) const;

	private:
		void 								parse_desktop_file(void);
		const std::string					property_value_by_key(std::string key) const;

		log4cxx::LoggerPtr					logger;
		std::map<std::string, std::string>	properties;
};

#endif
