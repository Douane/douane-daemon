#include "network_activity.h"

NetworkActivity::NetworkActivity(struct network_activity * activity, ProcessesManager * processes_manager)
: logger(log4cxx::Logger::getLogger("NetworkActivity")),
  processes_manager(processes_manager)
{
  this->devise_name   = activity->devise_name;
  this->protocol      = this->protocol_name_from_id(activity->protocol);
  this->ip_source     = activity->ip_source;
  this->port_source   = activity->port_source;
  this->ip_destination  = activity->ip_destination;
  this->port_destination  = activity->port_destination;
  this->size        = activity->size;

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
  this->hostname          = activity.hostname;
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

std::string NetworkActivity::protocol_name_from_id(int id) const
{
  switch(id)
  {
    case IPPROTO_TCP: return "TCP";
    case IPPROTO_UDP: return "UDP";
    case IPPROTO_ICMP: return "ICMP";
    case IPPROTO_IGMP: return "IGMP";
    default:
      std::stringstream os;
      os << "UNKNOWN id " << id;
      return os.str();
  }
}

const std::string NetworkActivity::append_port_code(void) const
{
  switch(this->port_destination)
  {
    case 20: return " (ftp-data)";
    case 21: return " (ftp)";
    case 22: return " (ssh)";
    case 23: return " (telnet)";
    case 25: return " (smtp)";
    case 52: return " (xns)";
    case 53: return " (dns)";
    case 80: return " (http)";
    case 101: return " (hostname)";
    case 263: return " (hdap)";
    case 443: return " (https)";
    default: return "";
  }
}

void NetworkActivity::request_resolve_remote_hostname(void)
{
  this->dns_client = new DnsClient(this->ip_destination);

  // When DnsClient emit result_received signal then fire NetworkActivity::update_hostname
  this->update_signal = this->dns_client->on_result_received_connect(boost::bind(&NetworkActivity::update_hostname, this, _1, _2));
  this->dns_client->find_or_resolve_by_ip_address();
}

void NetworkActivity::update_hostname(const std::string requested_address, const std::string resolved_hostname)
{
  std::cout << "NetworkActivity::update_hostname(\"" << requested_address << "\", \"" << resolved_hostname << "\")" << std::endl;

  if (requested_address != this->ip_destination)
    return;

  this->hostname = resolved_hostname;
  if (this->hostname_updated.num_slots() > 0)
    this->hostname_updated(this->ip_destination, this->hostname);

  if (this->update_signal.connected())
  {
    this->update_signal.disconnect();

    if (this->dns_client != NULL)
      delete this->dns_client;
  }
}

const std::string NetworkActivity::hostname_or_ip_destination(void) const
{
  return this->hostname == "" ? this->ip_destination : this->hostname;
}

boost::signals2::connection NetworkActivity::on_hostname_updated_connect(const signalHostnameUpdatedType &slot)
{
  return hostname_updated.connect(slot);
}

NetworkActivity::signalHostnameUpdated NetworkActivity::hostname_updated;
