#ifndef DESKTOP_FILES_H
#define DESKTOP_FILES_H

#include <log4cxx/logger.h>
#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include "desktop_file.h"

#define DESKTOP_FILES_ROOT_PATH "/usr/share/applications/"

class DesktopFiles
{
  public:
    /*
    ** Constructors and Destructor
    */
    DesktopFiles();
    virtual ~DesktopFiles();

    /*
    ** Instance methods
    */
    const DesktopFile *                 find_desktop_file_by_application_name(const std::string name) const;

  private:
    log4cxx::LoggerPtr                  logger;
    std::map<std::string, DesktopFile>  desktop_files;

    void                                load_desktop_files(void);
};

#endif
