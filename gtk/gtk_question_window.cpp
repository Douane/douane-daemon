#include "gtk_question_window.h"

GtkQuestionWindow::GtkQuestionWindow(const Glib::RefPtr<Gtk::Application> &application)
: is_shown(false),
  m_Notebook(Gtk::manage(new Gtk::Notebook())),
  logger(log4cxx::Logger::getLogger("GtkQuestionWindow")),
  application(application),
  accept_new_activities(true),
  has_received_activities(false)
{
	pthread_mutex_init(&mutex, NULL);

	// Define Gtk::Window options
	this->set_title("Douane");
	this->set_resizable(false);
	this->set_size_request(600, 200);
	this->set_position(Gtk::WIN_POS_CENTER);

	// Create and define Gtk::Notebook options
	this->m_Notebook->set_border_width(3);
	this->m_Notebook->set_tab_pos(Gtk::POS_LEFT);
	this->m_Notebook->set_scrollable(true);
	this->add(*this->m_Notebook);

	// This signal is used in order to hide the window as soon as all Gtk::Notebook pages are removed
	this->m_Notebook->signal_page_removed().connect(sigc::mem_fun(*this, &GtkQuestionWindow::on_page_removed));

	// When GtkBoxUnknownApplication emit new_rule_validated signal then fire GtkQuestionWindow::after_validate_rule
	GtkBoxUnknownApplication::on_new_rule_validated_connect(boost::bind(&GtkQuestionWindow::after_validate_rule, this, _1, _2));

	this->show_all();
}

GtkQuestionWindow::~GtkQuestionWindow()
{
	pthread_mutex_destroy(&mutex);
}

bool GtkQuestionWindow::on_visibility_notify_event(GdkEventVisibility* event)
{
	// That boolean is used to determine if the window is visible or not
	// It helps to define if the Gtk::Window has finish to boot or not
	this->is_shown = (event->state == GDK_VISIBILITY_UNOBSCURED);

	// Hide the window until at least one network activity received
	if (!this->has_received_activities)
		this->hide_question_window();

	return true;
}

bool GtkQuestionWindow::on_delete_event(GdkEventAny *)
{
	this->hide_question_window();

	// Stop propagation
	return true;
}

void GtkQuestionWindow::on_page_removed(Widget*, guint)
{
	if (this->m_Notebook->get_n_pages() == 0)
		this->hide_question_window();
}

void GtkQuestionWindow::after_validate_rule(const NetworkActivity * activity, bool allowed)
{
	this->new_rule_validated(activity, allowed);
}


bool GtkQuestionWindow::hide_question_window(void)
{
	// Reject all new activities during the time we hide the window
	this->accept_new_activities = false;

	// It is mandatory to call hold() on the application in order to not close it
	this->application->hold();
	this->hide();

	// Allow again to receive activities (will unhide the question window)
	this->accept_new_activities = true;

	// Stop call to that method from Glib::signal_idle()
	return false;
}

bool GtkQuestionWindow::show_question_window(void)
{
	// Ensure the window is not alreadu visible
	if (!this->get_visible())
		this->set_visible();
	return false;
}

void GtkQuestionWindow::unhide(void)
{
	// Call set_visible as soon as Gtk main loop is free
	Glib::signal_idle().connect(sigc::mem_fun(*this, &GtkQuestionWindow::show_question_window));
}

void GtkQuestionWindow::add_activity(const NetworkActivity * activity)
{
	// This boolean is used as a kind of state machine
	// in order to allow actions only when having received at least one activity
	if (!this->has_received_activities)
		this->has_received_activities = true;

	if (this->accept_new_activities)
	{
		// Prevent to create 2 tabs for the same application if a lot of packets are coming in once
		ThreadLock thread_lock(mutex);

		std::map<std::string, const Process*>::const_iterator it = this->unknown_applications.find(activity->process->executable_sha256);

		// The NetworkActivity->Process point to an unknown application
		if (it == this->unknown_applications.end())
		{
			// Register the new unknown application
			this->unknown_applications.insert(std::make_pair(activity->process->executable_sha256, activity->process));

			// Create a box in a new tab of the Gtk::Notebook
			Gtk::manage(new GtkBoxUnknownApplication(activity))->append_to_notebook(this->m_Notebook);

			// Enqueue a request to unhide the question window
			this->unhide();
		}
	}
}

boost::signals2::connection GtkQuestionWindow::on_new_rule_validated_connect(const signalNewRuleValidatedType &slot)
{
	return GtkQuestionWindow::new_rule_validated.connect(slot);
}

GtkQuestionWindow::signalNewRuleValidated GtkQuestionWindow::new_rule_validated;