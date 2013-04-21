#include "process.h"

Process::Process(const pid_t pid)
: process_pid(pid),
  logger(log4cxx::Logger::getLogger("Process"))
{
	this->build_from_pid();
}

Process::Process(const Process &process)
: process_pid(process.process_pid),
  logger(log4cxx::Logger::getLogger("Process")),
  executable_sha256(process.executable_sha256)
{
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
	this->get_executable_name_from_path();
}

void Process::try_get_process_path(void)
{
	try
	{
		this->path = boost::filesystem::read_symlink(boost::filesystem::path(this->proc_link)).string();

		// This process is a new one, so complete it
		Tools tools;
		this->executable_sha256 = tools.make_sha256_from(this->path);
	} catch (boost::filesystem::filesystem_error error)
	{
		throw ProcessException("Unable to read link: " + this->proc_link + ": " + std::string(error.what()));
	}
}

void Process::get_executable_name_from_path(void)
{
	this->executable_name = boost::filesystem::path(this->path).filename().string();
}