#include "douane_external_dialog.h"

DouaneExternalDialog::DouaneExternalDialog(const std::string dialog_process_name)
: logger(log4cxx::Logger::getLogger("DouaneExternalDialog")),
  idle(true),
  dialog_process_name(dialog_process_name)
{
  LOG4CXX_DEBUG(logger, "DouaneExternalDialog initialized with process " << this->dialog_process_name);
}

DouaneExternalDialog::~DouaneExternalDialog(void)
{

}

void DouaneExternalDialog::popup(void)
{
  // In case the process is running
  if (DouaneExternalDialog::process_running())
  {
    LOG4CXX_DEBUG(logger, this->dialog_process_name << " is already running");
    return; // Exit the method
  }

  // Stupid lock
  if (idle)
  {
    idle = false;
    system(std::string(this->dialog_process_name + " & > /dev/null").c_str());
    idle = true;
  }
}

/*
** Private
*/

bool DouaneExternalDialog::process_running(void) const
{
  return 0 == system(std::string("pgrep " + this->dialog_process_name + " > /dev/null").c_str());
}
