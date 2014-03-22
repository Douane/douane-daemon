#ifndef DOUANE_EXTERNAL_DIALOG_H
#define DOUANE_EXTERNAL_DIALOG_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <log4cxx/logger.h>
#include "network_activity.h"

/*
** Interface to launch the external process which display
*  the question window where the user allow/deny a process activities.
*/
class DouaneExternalDialog
{
  public:
    /*
    ** Constructors and Destructor
    */
    DouaneExternalDialog(const std::string dialog_process_name);
    virtual ~DouaneExternalDialog(void);

    /*
    ** Instance methods
    */
    void                popup_if_needed(const NetworkActivity *);
    void                update_process_id(const std::string &process_id);

  private:
    log4cxx::LoggerPtr  logger;
    bool                idle;
    const std::string   dialog_process_name;
    std::string         current_process_id;

    /*
    ** Instance methods
    */
    bool                process_running(void) const;
};

#endif
