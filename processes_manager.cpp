#include "processes_manager.h"

ProcessesManager::ProcessesManager()
: logger(log4cxx::Logger::getLogger("ProcessesManager"))
{

}

ProcessesManager::~ProcessesManager()
{

}

void ProcessesManager::set_desktop_files(const DesktopFiles * desktop_files)
{
	this->desktop_files = desktop_files;
}

const Process * ProcessesManager::find_or_create_from_pid(pid_t process_id)
{
	try
	{
		// Build a new process. Process will try to find its path.
		Process process(process_id);

		// Now lookup for a registered process at this path
		std::map<std::string, Process>::const_iterator it = this->processes.find(process.path);
		if (it == this->processes.end())
		{
			const Process * new_process = this->make_new_process(process.path, process_id);

			// Return a pointer of that new process
			return new_process;
		} else {
			// This process is already known so return its pointer
			return &it->second;
		}
	} catch (ProcessException exception)
	{
		// Process raise due to a no such file or no enough permissions error
		return NULL;
	}
}

Process * ProcessesManager::make_new_process(std::string &path, pid_t process_id)
{
	Process process(path);

	// This process is a new one, so complete it
	process.process_pid = process_id;
	process.executable_name = boost::filesystem::path(process.path).filename().string();

	process.executable_sha256 = this->make_sha256_from(process.path);

	const DesktopFile * desktop_file = this->desktop_files->find_desktop_file_by_application_name(process.executable_name);
	if (desktop_file != NULL)
	{
		process.icon_name = desktop_file->get_icon();
		process.printable_name = desktop_file->get_name();
	} else {
		process.printable_name = "";
	}

	if (process.icon_name == "")
		process.icon_name = "image-missing";

	// Save that new process
	this->processes.insert(std::make_pair(process.path, process));

	return &this->processes.find(process.path)->second;
}

const std::string ProcessesManager::executable_to_string(std::string &path) const
{
	std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
	if (in)
	{
		std::ostringstream contents;
		contents << in.rdbuf();
		in.close();
		return(contents.str());
	}
	throw(errno);
}

const std::string ProcessesManager::make_sha256_from(std::string &path) const
{
	const std::string file_content = this->executable_to_string(path);

	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, file_content.c_str(), file_content.size());
	SHA256_Final(hash, &sha256);
	std::stringstream ss;
	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
	    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	return ss.str();
}