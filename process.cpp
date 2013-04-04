#include "process.h"

Process::Process(int pid)
: logger(log4cxx::Logger::getLogger("Process"))
{
	this->process_pid = pid;
	this->build_from_pid();
}

Process::Process(std::string &path)
: path(path),
  logger(log4cxx::Logger::getLogger("Process"))
{

}

Process::Process(const Process &process)
: logger(log4cxx::Logger::getLogger("Process"))
{
	this->process_pid		= process.process_pid;
	this->executable_sha256	= process.executable_sha256;
	this->path				= process.path;
	this->executable_name	= process.executable_name;
	this->icon_name			= process.icon_name;
	this->printable_name	= process.printable_name;
	this->proc_link			= process.proc_link;
}

Process::~Process()
{

}

void Process::build_from_pid(void)
{
	if (this->process_pid == 0)
		throw ProcessException("PID is 0");

	this->proc_link = "/proc/" + boost::lexical_cast<std::string>(this->process_pid) + "/exe";

	this->try_get_process_path();
}

void Process::try_get_process_path(void)
{
	try
	{
		this->path = boost::filesystem::read_symlink(boost::filesystem::path(this->proc_link)).string();
	} catch (boost::filesystem::filesystem_error error)
	{
		throw ProcessException("Unable to read link: " + this->proc_link + ": " + std::string(error.what()));
	}
}