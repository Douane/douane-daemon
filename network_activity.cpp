#include "network_activity.h"

NetworkActivity::NetworkActivity(struct network_activity * activity, ProcessesManager * processes_manager)
: logger(log4cxx::Logger::getLogger("NetworkActivity")),
  processes_manager(processes_manager)
{
  this->devise_name       = activity->devise_name;
  this->protocol          = this->protocol_name_from_id(activity->protocol);
  this->ip_source         = activity->ip_source;
  this->port_source       = activity->port_source;
  this->ip_destination    = activity->ip_destination;
  this->port_destination  = activity->port_destination;
  this->size              = activity->size;

  this->process = this->processes_manager->find_or_create_from_path(activity->process_path);
}

NetworkActivity::NetworkActivity(const NetworkActivity &activity)
: logger(log4cxx::Logger::getLogger("NetworkActivity"))
{
  this->devise_name       = activity.devise_name;
  this->protocol          = activity.protocol;
  this->ip_source         = activity.ip_source;
  this->port_source       = activity.port_source;
  this->ip_destination    = activity.ip_destination;
  this->port_destination  = activity.port_destination;
  this->size              = activity.size;
  this->process           = activity.process;
}

NetworkActivity::~NetworkActivity(void)
{

}

bool NetworkActivity::process_has_been_detected(void) const
{
  return this->process != NULL;
}

const std::string NetworkActivity::str(void) const
{
  std::stringstream os;

  os << "[" << this->devise_name << "] " << this->protocol << " ";
  os << this->ip_source << ":" << this->port_source;
  os << " -> ";
  os << this->ip_destination << ":" << this->port_destination;
  os << " with a size of " << this->size;

  if (this->process)
    os << " for process " << this->process->path;

  return os.str();
}

const std::string NetworkActivity::append_port_code(void) const
{
  switch(this->port_destination)
  {
    case 20:  return " (ftp-data)";
    case 21:  return " (ftp)";
    case 22:  return " (ssh)";
    case 23:  return " (telnet)";
    case 25:  return " (smtp)";
    case 52:  return " (xns)";
    case 53:  return " (dns)";
    case 80:  return " (http)";
    case 101: return " (hostname)";
    case 263: return " (hdap)";
    case 443: return " (https)";
    default:  return "";
  }
}

/*
** Private
*/

std::string NetworkActivity::protocol_name_from_id(int id) const
{
  switch(id)
  {
    case IPPROTO_TCP:  return "TCP";
    case IPPROTO_UDP:  return "UDP";
    case IPPROTO_ICMP: return "ICMP";
    case IPPROTO_IGMP: return "IGMP";
    default:
      std::stringstream os;
      os << "UNKNOWN id " << id;
      return os.str();
  }
}
