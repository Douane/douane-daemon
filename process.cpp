#include "process.h"

Process::Process(std::string path)
: path(path),
  logger(log4cxx::Logger::getLogger("Process"))
{
  this->build_from_path();
}

Process::Process(const Process &process)
: logger(log4cxx::Logger::getLogger("Process"))
{
  this->path              = process.path;
  this->executable_name   = process.executable_name;
  this->icon_name         = process.icon_name;
  this->printable_name    = process.printable_name;
  this->executable_sha256 = process.executable_sha256;
}

Process::~Process(void)
{

}

const std::string Process::get_executable_sha256(void) const
{
  return this->executable_sha256;
}

void Process::update_executable_sha256(void)
{
  Tools tools;
  this->executable_sha256 = tools.make_sha256_from(this->path);
}

/*
** Private
*/

void Process::build_from_path(void)
{
  this->get_executable_name_from_path();
}

void Process::get_executable_name_from_path(void)
{
  this->executable_name = boost::filesystem::path(this->path).filename().string();
}
