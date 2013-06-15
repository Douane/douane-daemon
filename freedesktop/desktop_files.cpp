#include "desktop_files.h"

DesktopFiles::DesktopFiles(void)
: logger(log4cxx::Logger::getLogger("DesktopFiles"))
{
  this->load_desktop_files();
  LOG4CXX_INFO(logger, "Loaded " << this->desktop_files.size() << " Freedesktop desktop files");
}

DesktopFiles::~DesktopFiles(void)
{

}

void DesktopFiles::load_desktop_files(void)
{
  // Iterate over all files from /usr/share/applications/
  boost::filesystem::directory_iterator end_iter;
  for (boost::filesystem::directory_iterator dir_itr(DESKTOP_FILES_ROOT_PATH);
    dir_itr != end_iter;
    ++dir_itr)
  {
    // Only Iterate over *.desktop files
    if (boost::filesystem::extension(*dir_itr) == ".desktop")
    {
      try
      {
        // Only work with files (ignore sub folders)
        if (boost::filesystem::is_regular_file(dir_itr->status()))
        {
          // Build a new DesktopFile for that path
          DesktopFile desktop_file(dir_itr->path());

          if (desktop_file.get_exec() != "")
          {
            // Save it to the desktop file Hash
            this->desktop_files.insert(std::make_pair(desktop_file.get_exec(), desktop_file));
          }
        }
      } catch (const std::exception & ex) {
        LOG4CXX_ERROR(logger, dir_itr->path().filename() << " " << ex.what());
      }
    }
  }
}

const DesktopFile * DesktopFiles::find_desktop_file_by_application_name(const std::string name) const
{
  /**
   *  Build a regular expression matching the application name
   *  then a space and the anything.
   *
   *  The goal is to identify application when in the desktop file
   *  it has arguments in the Exec= field
   */
  boost::regex expression(name + "\\s.*");

  // Iterate over each keys
  for (std::map<std::string, DesktopFile>::const_iterator iter = this->desktop_files.begin();
    iter != this->desktop_files.end();
    ++iter)
  {
    // When the regex match
    if(regex_search(iter->first, expression))
    {
      // Return the stored DesktopFile
      return &iter->second;
      break;
    }
  }

  // If nothing found that the given name return a NULL
  return NULL;
}
