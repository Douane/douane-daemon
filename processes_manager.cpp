#include "processes_manager.h"

ProcessesManager::ProcessesManager(void)
: logger(log4cxx::Logger::getLogger("ProcessesManager"))
{

}

ProcessesManager::~ProcessesManager(void)
{

}

void ProcessesManager::set_desktop_files(const DesktopFiles * desktop_files)
{
  this->desktop_files = desktop_files;
}

const Process * ProcessesManager::find_or_create_from_path(const std::string & path)
{
  try
  {
    // Build a new process for the given path
    Process process(path);

    // Now lookup for a registered process at this path
    std::map<std::string, Process>::const_iterator it = this->processes.find(process.path);
    if (it == this->processes.end())
    {
      const Process * new_process = this->make_new_process(&process);

      // Return a pointer of that new process
      return new_process;
    } else {
      // This process is already known so return its pointer
      return &it->second;
    }
  } catch (const ProcessException &exception)
  {
    // Process raise due to a no such file or no enough permissions error
    LOG4CXX_ERROR(logger, "Unable to build process for path " << path << ": " << exception.what());
    return NULL;
  }
}

/*
** Private
*/

Process * ProcessesManager::make_new_process(Process * new_process)
{
  const DesktopFile * desktop_file = this->desktop_files->find_desktop_file_by_application_name(new_process->executable_name);

  // Detect application icon and name
  if (desktop_file == NULL)
  {
    new_process->printable_name = "";
  } else {
    new_process->icon_name = desktop_file->get_icon();
    new_process->printable_name = desktop_file->get_name();
  }

  // Calculate the process SHA256
  new_process->update_executable_sha256();

  // Default icon (when no one defined in a DesktopFile)
  if (new_process->icon_name == "")
    new_process->icon_name = "image-missing";

  // Save that new process
  this->processes.insert(std::pair<const std::string&, const Process&>(new_process->path, *new_process));

  return &this->processes.find(new_process->path)->second;
}
