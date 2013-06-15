#include "gtk_box_unknown_application.h"

/**
 *  The constructor forces to create a copy of the NetworkActivity (and of the Process)
 *  as this object will be called later on user action (when user will click on the Allow or Deny button).
 *  At this moment the NetworkActivity (and so the related Process) passed in parameter will no more exists.
 */
GtkBoxUnknownApplication::GtkBoxUnknownApplication(const NetworkActivity * activity)
: logger(log4cxx::Logger::getLogger("GtkBoxUnknownApplication")),
  activity(NetworkActivity(*activity)),
  destroying(false)
{
	this->hostname_updated_signal = NetworkActivity::on_hostname_updated_connect(boost::bind(&GtkBoxUnknownApplication::update_application_hostname_or_ip_destination, this, _1, _2));
}

GtkBoxUnknownApplication::~GtkBoxUnknownApplication()
{
	this->destroying = true;

	// Disconnect signals before do destroy that object
	if (this->hostname_updated_signal.connected())
		this->hostname_updated_signal.disconnect();
}

void GtkBoxUnknownApplication::append_to_notebook(Gtk::Notebook * notebook)
{
	if (this->destroying)
		return;

	this->notebook = notebook;
	this->build_box();
}

void GtkBoxUnknownApplication::build_box(void)
{
	if (this->destroying)
		return;

	Gtk::Fixed * fixed_background = Gtk::manage(new Gtk::Fixed());

	/**
	 *  The Gtk::Box vertical with labels and buttons
	 */
	Gtk::Box * vbox_application_info_and_buttons = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
	// Application title
	Gtk::Label * label_application_title = Gtk::manage(new Gtk::Label());
	// Show name from DesktopFile otherwise executable name
	std::string printable_name = this->activity.process->printable_name == "" ? this->activity.process->executable_name : this->activity.process->printable_name;
	label_application_title->set_markup("<span size=\"40000\">" + printable_name + "</span>");
	label_application_title->set_alignment(0.0, 0.0);
	label_application_title->set_margin_left(17);
	label_application_title->set_margin_top(17);
	vbox_application_info_and_buttons->pack_start(*label_application_title, false, true, 0);


	// Application action description
	Gtk::Box * vbox_application_description = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));

	Gtk::Label * label_wants_to_connect = Gtk::manage(new Gtk::Label("wants to connect to "));
	label_wants_to_connect->set_alignment(0.0, 0.0);
	label_wants_to_connect->set_margin_left(18);
	label_wants_to_connect->set_margin_top(8);
	vbox_application_description->pack_start(*label_wants_to_connect, false, true, 0);

	this->label_hostname_or_ip_destination = Gtk::manage(new Gtk::Label());
	this->label_hostname_or_ip_destination->set_markup("<b>" + this->activity.hostname_or_ip_destination() + "</b>");
	this->label_hostname_or_ip_destination->set_tooltip_text(this->activity.ip_destination);
	this->label_hostname_or_ip_destination->set_selectable(true);
	this->label_hostname_or_ip_destination->set_margin_top(8);
	vbox_application_description->pack_start(*this->label_hostname_or_ip_destination, false, true, 0);

	Gtk::Label * label_application_action = Gtk::manage(new Gtk::Label());

	std::stringstream application_description;
	application_description << " on ";
	application_description << this->activity.protocol;
	application_description << " port <b>";
	application_description << this->activity.port_destination;
	application_description << "</b>";
	application_description << this->activity.append_port_code();
	application_description << ".";

	label_application_action->set_markup(application_description.str());
	label_application_action->set_margin_top(8);
	vbox_application_description->pack_start(*label_application_action, false, true, 0);

	vbox_application_info_and_buttons->pack_start(*vbox_application_description, false, true, 0);

	Gtk::Label * label_application_pid = Gtk::manage(new Gtk::Label());

	std::stringstream application_pid;
	application_pid << "Application path <b>";
	application_pid << this->activity.process->path;
	application_pid << "</b>.";

	label_application_pid->set_markup(application_pid.str());
	label_application_pid->set_alignment(0.0, 0.0);
	label_application_pid->set_margin_left(18);
	label_application_pid->set_margin_top(8);
	vbox_application_info_and_buttons->pack_start(*label_application_pid, false, true, 0);

	// Allow and Deny buttons
	Gtk::ButtonBox * buttonbox_allow_deny = Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_HORIZONTAL));
	buttonbox_allow_deny->set_layout(Gtk::BUTTONBOX_START);
	buttonbox_allow_deny->set_spacing(50);
	buttonbox_allow_deny->set_border_width(20);
	Gtk::Button * button_allow = Gtk::manage(new Gtk::Button("Allow"));
	button_allow->set_size_request(100);
	button_allow->signal_clicked().connect(sigc::mem_fun(*this, &GtkBoxUnknownApplication::on_allow_button_clicked));
	buttonbox_allow_deny->pack_start(*button_allow, false, true, 0);
	Gtk::Button * button_deny = Gtk::manage(new Gtk::Button("Deny"));
	button_deny->set_size_request(100);
	button_deny->signal_clicked().connect(sigc::mem_fun(*this, &GtkBoxUnknownApplication::on_deny_button_clicked));
	buttonbox_allow_deny->pack_start(*button_deny, false, true, 0);
	vbox_application_info_and_buttons->pack_start(*buttonbox_allow_deny, true, true, 0);

	// About button
	Gtk::Button * button_allow = Gtk::manage(new Gtk::Button("Allow"));

	Glib::ustring icon_name(this->activity.process->icon_name);

	/**
	 *  The huge Gtk::Image of the icon with an alpha
	 */
	GtkApplicationIconSublimer * application_icon_sublimer = Gtk::manage(new GtkApplicationIconSublimer());
	application_icon_sublimer->set_from_icon_name(icon_name);

	fixed_background->put(*application_icon_sublimer, 350, 0);
	fixed_background->put(*vbox_application_info_and_buttons, 0, 0);

	this->pack_start(*fixed_background, true, true, 0);

	// Load application icon to put it in place of the tab label
	Gtk::Image * icon = Gtk::manage(new Gtk::Image());
	icon->set_from_icon_name(icon_name, Gtk::IconSize(Gtk::ICON_SIZE_DND));

	// Create a new tab with the icon
	this->notebook->append_page(*this, *icon);
	this->notebook->show_all_children();
}

void GtkBoxUnknownApplication::on_allow_button_clicked(void)
{
	this->make_rule_and_remove_box(true);
}

void GtkBoxUnknownApplication::on_deny_button_clicked(void)
{
	this->make_rule_and_remove_box(false);
}

void GtkBoxUnknownApplication::make_rule_and_remove_box(bool allowed)
{
	this->new_rule_validated(&this->activity, allowed);
	this->notebook->remove_page(*this);
}

bool GtkBoxUnknownApplication::update_application_hostname_or_ip_destination_idle(void)
{
	std::cout << "GtkBoxUnknownApplication::update_application_hostname_or_ip_destination_idle() BEFORE" << std::endl;
	if (this->destroying)
		return false;

	this->label_hostname_or_ip_destination->set_markup("<b>" + this->activity.hostname_or_ip_destination() + "</b>");
	std::cout << "GtkBoxUnknownApplication::update_application_hostname_or_ip_destination_idle() AFTER" << std::endl;
	return false; // Don't loop on that method
}

void GtkBoxUnknownApplication::update_application_hostname_or_ip_destination(const std::string &ip_address, const std::string &hostname)
{
	std::cout << "GtkBoxUnknownApplication::update_application_hostname_or_ip_destination(\"" << ip_address << "\", \"" << hostname << "\"" << std::endl;

	if (this->destroying)
		return;

	std::cout << "Comparing this->activity.ip_destination: \"" << this->activity.ip_destination << "\" with \"" << ip_address << "\"" << std::endl;
	if (this->activity.ip_destination == ip_address)
	{
		std::cout << "Message is for me." << std::endl;
		this->activity.hostname = hostname;
		std::cout << "New hostname: " << this->activity.hostname << std::endl;
		std::cout << "I plan to update the GUI..." << std::endl;
		Glib::signal_idle().connect(sigc::mem_fun(*this, &GtkBoxUnknownApplication::update_application_hostname_or_ip_destination_idle));

		std::cout << "Disconnecting from updates as I'm updated!" << std::endl;
		this->hostname_updated_signal.disconnect();
		std::cout << "All is done!" << std::endl;
	} else {
		std::cout << "Message isn't for me." << std::endl;
	}
}

boost::signals2::connection GtkBoxUnknownApplication::on_new_rule_validated_connect(const signalNewRuleValidatedType &slot)
{
	return GtkBoxUnknownApplication::new_rule_validated.connect(slot);
}

GtkBoxUnknownApplication::signalNewRuleValidated GtkBoxUnknownApplication::new_rule_validated;