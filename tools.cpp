#include "tools.h"

Tools::Tools(void)
: logger(log4cxx::Logger::getLogger("Tools"))
{

}

Tools::~Tools(void)
{

}

const std::string Tools::make_sha256_from(const std::string & path) const
{
  const std::string file_content = this->executable_to_string(path);

  if (!file_content.empty())
  {
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
  } else {
    return std::string("");
  }
}

bool Tools::is_number(const std::string& s) const
{
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}

const std::string Tools::douane_root_path(void) const
{
  char    result[PATH_MAX];
  readlink("/proc/self/exe", result, PATH_MAX);
  const char * root_path = dirname(result);

  LOG4CXX_DEBUG(logger, "Douane root path is " << root_path);

  // Ensure folder exists
  mkdir(root_path, 0700);

  return std::string(root_path);
}

/*
** Private
*/

const std::string Tools::executable_to_string(const std::string & path) const
{
  std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
  if (in)
  {
    std::ostringstream contents;
    contents << in.rdbuf();
    in.close();
    return(contents.str());
  }
  LOG4CXX_ERROR(logger, "On opening " << path << ": " << strerror(errno));
  return std::string("");
}
