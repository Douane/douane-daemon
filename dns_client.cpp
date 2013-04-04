#include "dns_client.h"

DnsClient::DnsClient(const std::string &ip_address)
: logger(log4cxx::Logger::getLogger("DnsClient")),
  resolving_ip_address(ip_address),
  destroying(false),
  already_disconnected(false)
{
	std::cout << "DnsClient::DnsClient(\"" << ip_address << "\")" << std::endl;
}

DnsClient::~DnsClient()
{
	std::cout << "DnsClient::~DnsClient()" << std::endl;
	this->destroying = true;

	std::cout << "DnsClient::~DnsClient(): Calling clear_signals() from destructor" << std::endl;
	this->clear_signals();
}

void DnsClient::clear_signals(void)
{
	if (this->already_disconnected)
	{
		std::cout << "DnsClient::clear_signals(): Already disconnected!" << std::endl;
		return;
	} else {
		this->already_disconnected = true;
	}

	std::cout << "DnsClient::clear_signals(\"" << this->resolving_ip_address << "\")" << std::endl;
	if (this->resolved_address_found_signal.connected())
	{
		std::cout << "DnsClient::~DnsClient(): Disconnecting resolved_address_found_signal..." << std::endl;
		this->resolved_address_found_signal.disconnect();
	}

	if (this->resolved_address_not_found_signal.connected())
	{
		std::cout << "DnsClient::~DnsClient(): Disconnecting resolved_address_not_found_signal..." << std::endl;
		this->resolved_address_not_found_signal.disconnect();
	}
}

void DnsClient::resolved_address_found(const std::string requested_address, const std::string resolved_address)
{
	std::cout << "DnsClient::resolved_address_found(\"" << requested_address << "\", \"" << resolved_address << "\")" << std::endl;

	if (this->destroying)
	{
		std::cout << "resolved_address_found Quitting..." << std::endl;
		std::cout << "DnsClient::resolved_address_found(\"" << requested_address << "\"): Quitting..." << std::endl;
		return;
	}

	if (this->result_received.num_slots() == 0)
	{
		std::cout << "DnsClient::resolved_address_found(\"" << requested_address << "\"): No more connections for result_received signal" << std::endl;
		this->clear_signals();
		return;
	}

	if (this->resolving_ip_address == requested_address)
		this->result_received(requested_address, resolved_address);
}

void DnsClient::resolved_address_not_found(const std::string requested_address)
{
	if (this->destroying)
	{
		std::cout << "resolved_address_not_found Quitting..." << std::endl;
		std::cout << "DnsClient::resolved_address_not_found(\"" << requested_address << "\"): Quitting..." << std::endl;
		return;
	}

	if (this->resolving_ip_address == requested_address)
	{
		// Disconnect from signal as we have been notified
		std::cout << "DnsClient::~DnsClient(\"" << requested_address << "\"): Calling clear_signals() from DnsClient::resolved_address_not_found()" << std::endl;
		this->clear_signals();
		// Now resolve ip address
		this->resolve_ip_address();
	}
}

void DnsClient::find_or_resolve_by_ip_address(void)
{
	// When DnsCache emit resolved_address_found signal then fire DnsClient::resolved_address_found
	this->resolved_address_found_signal = DnsCache::on_resolved_address_found_connect(boost::bind(&DnsClient::resolved_address_found, this, _1, _2));
	// When DnsCache emit resolved_address_not_found signal then fire DnsClient::resolved_address_not_found
	this->resolved_address_not_found_signal = DnsCache::on_resolved_address_not_found_connect(boost::bind(&DnsClient::resolved_address_not_found, this, _1));

	if (this->looking_up_for_new_address.num_slots() > 0)
		this->looking_up_for_new_address(this->resolving_ip_address);
}

void DnsClient::resolve_ip_address(void)
{
	std::cout << "DnsClient::resolve_ip_address()" << std::endl;

	std::cout << "IpAddressTranslator::translate() BEFORE" << std::endl;
	std::string hostname(IpAddressTranslator::translate(this->resolving_ip_address));
	std::cout << "IpAddressTranslator::translate() AFTER: \"" << hostname << "\"" << std::endl;

	// Send result back to the DnsCache
	if (this->ip_address_resolution_done.num_slots() > 0)
	{
		std::cout << "DnsClient::resolve_ip_address(): Fireing " << this->ip_address_resolution_done.num_slots() << " ip_address_resolution_done..." << std::endl;
		this->ip_address_resolution_done(this->resolving_ip_address, hostname);
	} else {
		std::cout << "DnsClient::resolve_ip_address(\"" << this->resolving_ip_address << "\"): Will not Fire ip_address_resolution_done." << std::endl;
	}

	// Send result to NetworkActivity instances whom requested name resolution
	if (this->result_received.num_slots() > 0)
	{
		std::cout << "DnsClient::resolve_ip_address(): Fireing " << this->result_received.num_slots() << " result_received..." << std::endl;
		this->result_received(this->resolving_ip_address, hostname);
	} else {
		std::cout << "DnsClient::resolve_ip_address(): Will not Fire result_received." << std::endl;
	}

	std::cout << "DnsClient::~DnsClient(): Calling clear_signals() from DnsClient::resolve_ip_address()" << std::endl;
	this->clear_signals();
}

boost::signals2::connection DnsClient::on_looking_up_for_new_address_connect(const signalLookingUpForNewAddressType &slot)
{
	return looking_up_for_new_address.connect(slot);
}

DnsClient::signalLookingUpForNewAddress DnsClient::looking_up_for_new_address;

boost::signals2::connection DnsClient::on_result_received_connect(const signalResultReceivedType &slot)
{
	return result_received.connect(slot);
}

DnsClient::signalResultReceived DnsClient::result_received;

boost::signals2::connection DnsClient::on_ip_address_resolution_done_connect(const signalIpAddressResolutionDoneType &slot)
{
	return ip_address_resolution_done.connect(slot);
}

DnsClient::signalIpAddressResolutionDone DnsClient::ip_address_resolution_done;