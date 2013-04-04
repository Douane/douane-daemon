#include "gtk_application_icon_sublimer.h"

GtkApplicationIconSublimer::GtkApplicationIconSublimer()
: logger(log4cxx::Logger::getLogger("GtkApplicationIconSublimer"))
{
	this->set_size_request(300, 250);
}

GtkApplicationIconSublimer::~GtkApplicationIconSublimer()
{

}

bool GtkApplicationIconSublimer::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (!this->application_icon)
		return false;

	Gtk::Allocation allocation = get_allocation();
	const int height = allocation.get_height();

	Gdk::Cairo::set_source_pixbuf(cr, this->application_icon, 0, (height - this->application_icon->get_height()));
	cr->paint_with_alpha(0.2);

	return true;
}

void GtkApplicationIconSublimer::set_from_icon_name(Glib::ustring const &icon_name)
{
	Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

	if (theme->has_icon(icon_name))
	{
		this->application_icon = theme->load_icon(icon_name, 400, Gtk::ICON_LOOKUP_USE_BUILTIN);
	}
}