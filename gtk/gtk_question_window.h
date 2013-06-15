#ifndef GTK_QUESTION_WINDOW_H
#define GTK_QUESTION_WINDOW_H

#include <boost/signals2.hpp>
#include <log4cxx/logger.h>
#include <gtkmm.h>
#include "../network_activity.h"
#include "gtk_box_unknown_application.h"
#include "../thread_lock.h"

class GtkQuestionWindow : public Gtk::Window
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
    GtkQuestionWindow(const Glib::RefPtr<Gtk::Application> &application);
    virtual ~GtkQuestionWindow(void);

    /*
    ** Instance methods
    */
    bool                                  on_visibility_notify_event(GdkEventVisibility* event);
    virtual bool                          on_delete_event(GdkEventAny *);
    void                                  on_page_removed(Widget*, guint);
    void                                  after_validate_rule(const NetworkActivity * activity, bool allowed);
    bool                                  hide_question_window(void);
    bool                                  show_question_window(void);
    void                                  add_activity(const NetworkActivity * activity);
    static boost::signals2::connection    on_new_rule_validated_connect(const signalNewRuleValidatedType &slot);

    /*
    ** Attributes
    */
    bool                                  is_shown;

  protected:
    Gtk::Notebook *                       m_Notebook;

  private:
    log4cxx::LoggerPtr                    logger;
    std::map<std::string, const Process*> unknown_applications;
    const Glib::RefPtr<Gtk::Application>  &application;
    bool                                  accept_new_activities;
    bool                                  has_received_activities;
    pthread_mutex_t                       mutex;

    void                                  unhide(void);

    static signalNewRuleValidated         new_rule_validated;
};

#endif
