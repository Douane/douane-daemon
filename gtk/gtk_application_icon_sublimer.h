#ifndef GTK_APPLICATION_ICON_SUBLIMER_H
#define GTK_APPLICATION_ICON_SUBLIMER_H

#include <log4cxx/logger.h>
#include <gtkmm.h>
#include <iostream>

class GtkApplicationIconSublimer : public Gtk::Image
{
  public:
    /*
    ** Constructors and Destructor
    */
    GtkApplicationIconSublimer(void);
    virtual ~GtkApplicationIconSublimer(void);

    /*
    ** Instance methods
    */
    void                      set(const Glib::RefPtr<Gdk::Pixbuf>& icon);

  protected:
    virtual bool              on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    Glib::RefPtr<Gdk::Pixbuf> application_icon;

  private:
    log4cxx::LoggerPtr        logger;
};

#endif
