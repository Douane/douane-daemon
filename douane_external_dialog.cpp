#include "douane_external_dialog.h"

DouaneExternalDialog::DouaneExternalDialog(const std::string dialog_process_name)
: logger(log4cxx::Logger::getLogger("DouaneExternalDialog")),
  idle(true),
  dialog_process_name(dialog_process_name),
  current_process_id("")
{
  LOG4CXX_DEBUG(logger, "DouaneExternalDialog initialized with process " << this->dialog_process_name);
}

DouaneExternalDialog::~DouaneExternalDialog(void)
{

}

void DouaneExternalDialog::popup_if_needed(const NetworkActivity *)
{
  // In case the process is running
  if (this->current_process_id.empty() && this->idle)
  {
    this->idle = false;
    system(std::string(this->dialog_process_name + " -D & > /dev/null").c_str());
    this->idle = true;
  } else {
    LOG4CXX_DEBUG(logger, this->dialog_process_name << " is already running");
    return; // Exit the method
  }
}

void DouaneExternalDialog::update_process_id(const std::string &process_id)
{
  this->current_process_id = process_id;
}
