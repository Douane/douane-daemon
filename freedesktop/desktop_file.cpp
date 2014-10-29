#include "desktop_file.h"

DesktopFile::DesktopFile(const boost::filesystem::path& path)
: path(path),
  logger(log4cxx::Logger::getLogger("DesktopFile"))
{
  LOG4CXX_DEBUG(logger, "Processing file " << this->path << " ...");
  this->parse_desktop_file();
}

DesktopFile::~DesktopFile(void)
{

}

void DesktopFile::parse_desktop_file(void)
{
  // Open the desktop file for read
  std::ifstream infile(this->path.string().c_str());

  boost::regex expression("([\\w-]+)=(.*)");

  // Iterate over each lines
  for(std::string line; getline(infile, line);)
  {
    // Extract and store each key value pair
    boost::smatch what;
    if(boost::regex_match(line, what, expression, boost::match_extra))
    {
      // Store only keys with value
      if (what[2] != "")
        this->properties.insert(std::make_pair(what[1], what[2]));
    }
  }

  // Finally close the opened file
  infile.close();
}

const std::string DesktopFile::property_value_by_key(std::string key) const
{
  std::map<std::string, std::string>::const_iterator itr = this->properties.find(key);
  if(itr == this->properties.end())
    return "";
  else
    return itr->second;
}

const std::string DesktopFile::get_icon(void) const
{
  return this->property_value_by_key("Icon");
}

const std::string DesktopFile::get_exec(void) const
{
  return this->property_value_by_key("Exec");
}

const std::string DesktopFile::get_name(void) const
{
  return this->property_value_by_key("Name");
}
