#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <stdio.h>
#include <string.h>         // strerror()
#include <errno.h>          // errno
#include <fstream>          // std::ifstream
#include <iomanip>          // setw(), setfill()
#include <openssl/sha.h>
#include <log4cxx/logger.h>

class Tools
{
  public:
    /*
    ** Constructors and Destructor
    */
    Tools();
    virtual ~Tools();

    /*
    ** Instance methods
    */
    const std::string   make_sha256_from(const std::string & path) const;
    bool                is_number(const std::string& s) const;

  private:
    log4cxx::LoggerPtr  logger;

    const std::string   executable_to_string(const std::string & path) const;
};

#endif
