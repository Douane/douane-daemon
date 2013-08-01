#ifndef GTK_PROCESS_ICON_H
#define GTK_PROCESS_ICON_H

#include <log4cxx/logger.h>
#include <gtkmm.h>

#define APPLICATION_LARGE_ICON_SIZE 400
#define APPLICATION_SMALL_ICON_SIZE 25

class GtkProcessIcon
{
  public:
    /*
    ** Constructors and Destructor
    */
    GtkProcessIcon(void);
    virtual ~GtkProcessIcon(void);

    /*
    ** Instance methods
    */
    void                        set_icon_from_process(const std::string &icon_name, const std::string &executable_name);
    Glib::RefPtr<Gdk::Pixbuf>   get_large_icon_pixbuf(void) const;
    Glib::RefPtr<Gdk::Pixbuf>   get_small_icon_pixbuf(void) const;

  private:
    log4cxx::LoggerPtr          logger;
    Glib::RefPtr<Gdk::Pixbuf>   application_large_icon;
    Glib::RefPtr<Gdk::Pixbuf>   application_small_icon;
};

#endif
