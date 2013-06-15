#ifndef GTK_BOX_UNKNOWN_APPLICATION_H
#define GTK_BOX_UNKNOWN_APPLICATION_H

#include <boost/signals2.hpp>
#include <log4cxx/logger.h>
#include <gtkmm.h>
#include "../network_activity.h"
#include "../rule.h"
#include "gtk_application_icon_sublimer.h"

class GtkBoxUnknownApplication : public Gtk::Box
{
  public:
    /*
    ** Signals
    */
    typedef boost::signals2::signal<void(const NetworkActivity*, bool allowed)> signalNewRuleValidated;
    typedef signalNewRuleValidated::slot_type                                   signalNewRuleValidatedType;

    /*
    ** Constructors and Destructor
    */
    GtkBoxUnknownApplication(const NetworkActivity * activity);
    virtual ~GtkBoxUnknownApplication(void);

    /*
    ** Instance methods
    */
    void                                append_to_notebook(Gtk::Notebook * notebook);
    // Signals methods
    static boost::signals2::connection  on_new_rule_validated_connect(const signalNewRuleValidatedType &slot);

  private:
    log4cxx::LoggerPtr                  logger;
    NetworkActivity                     activity;
    boost::signals2::connection         hostname_updated_signal;
    Gtk::Notebook *                     notebook;
    Gtk::Label *                        label_hostname_or_ip_destination;
    bool                                destroying;

    void                                build_box(void);
    void                                on_allow_button_clicked(void);
    void                                on_deny_button_clicked(void);
    void                                make_rule_and_remove_box(bool allowed);
    bool                                update_application_hostname_or_ip_destination_idle(void);
    void                                update_application_hostname_or_ip_destination(const std::string &ip_address, const std::string &hostname);

    static signalNewRuleValidated       new_rule_validated;
};

#endif
