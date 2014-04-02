#include "netlink_listener.h"

NetlinkListener::NetlinkListener(void)
: logger(log4cxx::Logger::getLogger("NetlinkListener"))
{

}

NetlinkListener::~NetlinkListener(void)
{

}

void NetlinkListener::set_processes_manager(ProcessesManager * processes_manager)
{
  this->processes_manager = processes_manager;
}

void NetlinkListener::start(void)
{
  struct msghdr             msg;
  struct iovec              iov;
  struct network_activity * activity_struct = (struct network_activity *) malloc(sizeof(struct network_activity));
  int                       payload_size = NLMSG_SPACE(sizeof(struct network_activity));

  this->send_hand_check();

  LOG4CXX_DEBUG(logger, "Payload size is " << payload_size);

  LOG4CXX_DEBUG(logger, "Entering LKM communication loop...");
  this->running = true;
  while (this->running)
  {
    // nlh is malloc'ed and free'ed for each messages from the kernel
    struct nlmsghdr * nlh = (struct nlmsghdr *) malloc(payload_size);

    // nlh initialization
    memset(nlh, 0, payload_size);
    nlh->nlmsg_len = payload_size;
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    // msg initialization
    iov.iov_base  = (void *)nlh;
    iov.iov_len   = nlh->nlmsg_len;

    memset(&msg, 0, sizeof(struct msghdr));
    msg.msg_name    = (void *)&this->destination_address;
    msg.msg_namelen = sizeof(struct sockaddr_nl);
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    if (recvmsg(this->sock_fd, &msg, MSG_WAITALL) > 0)
    {
      memset(activity_struct, 0, sizeof(struct network_activity));
      memcpy(activity_struct, NLMSG_DATA(nlh), sizeof(struct network_activity));

      if (std::string(activity_struct->process_path) == "")
      {
        LOG4CXX_WARN(logger, "Received activity_struct process_path is empty!");
      } else {
        /**
         *  Build a NetworkActivity using received data from the Linux Kernel Module
         *
         *  The NetlinkMessageHandler will execute all other operations into a thread
         *  in order to respond immediately to the Linux Kernel Module.
         *
         *  Otherwise Linux Kernel Module method netlink_unicast() will return some
         *  error EAGAIN (-11) as the socket will be locked when trying to send another
         *  network activity.
         */
        NetworkActivity *       network_activity = new NetworkActivity(activity_struct, this->processes_manager);
        NetlinkMessageHandler * netlink_message_handler = new NetlinkMessageHandler(network_activity);
        netlink_message_handler->start_and_detach();
      }
    } else {
      LOG4CXX_WARN(logger, "Wasn't able to receive the new activity: " << strerror(errno));
    }
    free(nlh);
  }
}

void NetlinkListener::die(void)
{
  this->running = false;
}

void NetlinkListener::send_rule(const Rule * rule)
{
  LOG4CXX_DEBUG(logger, "NetlinkListener::send_rule...");

  // Build a new message
  struct network_activity * activity = this->build_message(KIND_SENDING_RULE);

  if (rule->is_allowed())
  {
    LOG4CXX_DEBUG(logger, "NetlinkListener::send_rule ALLOW for " << rule->process_path);
  } else {
    LOG4CXX_DEBUG(logger, "NetlinkListener::send_rule DISALLOW for " << rule->process_path);
  }

  // Assign some values
  strcpy(activity->devise_name, "lo");
  std::copy(rule->process_path.begin(), rule->process_path.end(), activity->process_path);
  activity->allowed = rule->is_allowed() ? 1 : 0;

  // Send it to the Kernel module
  this->send_message(activity);
}

void NetlinkListener::delete_rule(const Rule * rule)
{
  // Build a new message
  struct network_activity * activity = this->build_message(KIND_DELETE_RULE);

  // Assign some values
  strcpy(activity->devise_name, "lo");
  std::copy(rule->process_path.begin(), rule->process_path.end(), activity->process_path);

  // Send it to the Kernel module
  this->send_message(activity);
}

void NetlinkListener::say_goodbye(void)
{
  // Build a new message
  struct network_activity * activity = this->build_message(KIND_GOODBYE);
  // Assign some values
  strcpy(activity->devise_name, "lo");
  // Send it to the Kernel module
  this->send_message(activity);
}

/*
** Signals methods
*/
boost::signals2::connection NetlinkListener::on_connected_to_kernel_module_connect(const signalConnectedToKernelModuleType &slot)
{
  return connected_to_kernel_module.connect(slot);
}

NetlinkListener::signalConnectedToKernelModule NetlinkListener::connected_to_kernel_module;

/*
** Private
*/

void NetlinkListener::send_hand_check(void)
{
  // Build a new message
  struct network_activity * activity = this->build_message(KIND_HAND_CHECK);
  // Assign some values
  strcpy(activity->devise_name, "lo");

  LOG4CXX_DEBUG(logger, "NetlinkListener is sending the hand check");

  // Send it to the Kernel module
  this->send_message(activity);
}

struct network_activity * NetlinkListener::build_message(int kind)
{
  LOG4CXX_DEBUG(logger, "NetlinkListener::build_message...");
  struct network_activity * activity = (struct network_activity *) malloc(sizeof(struct network_activity));

  memset(activity, 0, sizeof(struct network_activity));
  activity->kind = kind;

  return activity;
}

void NetlinkListener::send_message(struct network_activity * activity)
{
  struct iovec    iov;
  struct msghdr   msg;

  struct nlmsghdr * nlh = (struct nlmsghdr *) malloc(NLMSG_SPACE(sizeof(struct network_activity)));
  nlh->nlmsg_len = NLMSG_SPACE(sizeof(struct network_activity));
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = NLM_F_REQUEST; // Must be set on all request messages.
  nlh->nlmsg_flags = 0;

  memcpy(NLMSG_DATA(nlh), activity, sizeof(struct network_activity));

  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;

  memset(&msg, 0, sizeof(struct msghdr));
  msg.msg_name = (void *)&destination_address;
  msg.msg_namelen = sizeof(struct sockaddr_nl);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  if (sendmsg(this->sock_fd, &msg, 0) == -1)
  {
    LOG4CXX_ERROR(logger, "Error while calling sendmsg: " << strerror(errno));
    throw SocketException("Unable to send message");
  } else {
    // In the case of the message sent was the hand check
    if (activity->kind == KIND_HAND_CHECK)
      // Daemon is now connected with Kernel Module
      this->connected_to_kernel_module();
  }
}
