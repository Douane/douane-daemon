#include "gtk_application_icon_sublimer.h"

GtkApplicationIconSublimer::GtkApplicationIconSublimer(void)
: logger(log4cxx::Logger::getLogger("GtkApplicationIconSublimer"))
{
  this->set_size_request(300, 250);
}

GtkApplicationIconSublimer::~GtkApplicationIconSublimer(void)
{

}

bool GtkApplicationIconSublimer::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  if (!this->application_icon)
  {
    LOG4CXX_DEBUG(logger, "Got no icon..");
    return false;
  }

  Gtk::Allocation allocation = get_allocation();
  const int height = allocation.get_height();

  Gdk::Cairo::set_source_pixbuf(cr, this->application_icon, 0, (height - this->application_icon->get_height()));
  cr->paint_with_alpha(0.2);

  return true;
}

void GtkApplicationIconSublimer::set(const Glib::RefPtr<Gdk::Pixbuf>& icon)
{
  if (!icon)
  {
    LOG4CXX_DEBUG(logger, "GtkApplicationIconSublimer::set Got no icon..");
  }

  this->application_icon = icon;
}
