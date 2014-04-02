// Basic includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <boost/bind.hpp>
#include <signal.h>
// Log4cxx includes
#include <log4cxx/logger.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/patternlayout.h>

// Douane internal includes
#include "netlink_listener.h"
#include "freedesktop/desktop_files.h"
#include "rules_manager.h"
#include "processes_manager.h"
#include "dbus/dbus_server.h"

// In the case the Makefile didn't initialized the VERSION variable
// this code initialize it to "UNKNOWN"
#ifndef DOUANE_VERSION
#define DOUANE_VERSION "UNKNOWN"
#endif

// Daemon flags and paths
bool          enabled_debug = false;
bool          has_to_daemonize = false;
bool          has_to_write_pid_file = false;
const char *  pid_file_path = "/var/run/douaned.pid";
const char *  log_file_path = "/var/log/douane.log";

// Initialize the logger for the current file
log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Main");

/* Global initialization
**
*  This is needed in order to access it from the main() method
*  but also from others like handler()
*/
NetlinkListener netlink_listener;

/*
** Handle the exit signal
*/
void handler(int sig)
{
  LOG4CXX_INFO(logger, "Exiting Douane daemon with signal " << sig << "...");
  netlink_listener.say_goodbye();
  exit(1);
}

/*
** Create a daemon from the current instance
*
*  This is executed when passing argument -d
*/
void do_daemonize(void)
{
  // Create child process
  switch(fork())
  {
    case -1:
    {
      std::cerr << "Unable to create daemon process" << std::endl;
      exit(-1);
    }
    case 0:
    {
      //unmask the file mode
      umask(0);

      if(setsid() < 0)
      {
        std::cout << "ERROR: Unable to set new session" << std::endl;
        // Return failure
        exit(1);
      }
      break;
    }
    default:
    {
      // Parent process can quit
      exit(0);
    }
  }
}

/*
** Print the daemon version
*
*  This is executed when passing argument -v
*/
void do_version(void)
{
  std::cout << DOUANE_VERSION << std::endl;
  exit(1);
}

/*
** Print the help menu
*
*  This is executed when passing argument -h
*/
void do_help(void)
{
  std::cout << "TODO: Write help message" << std::endl;
  exit(1);
}

/*
** Create the PID file and write the PID
*
*  This is executed when passing argument -p
*/
void do_pidfile(const char * path)
{
  std::ofstream pid_file;
  pid_file.open(path);
  if(!pid_file.is_open())
  {
    printf("Unable to create the PID file: %s\n", strerror(errno));
    exit(-1);
  }
  pid_file << getpid() << std::endl;
  pid_file.close();
}

void do_from_options(std::string option, const char * optarg)
{
  if (option == "daemon")
  {
    has_to_daemonize = true;
  } else if (option == "version")
  {
    do_version();
  } else if (option == "help")
  {
    do_help();
  } else if (option == "pid-file")
  {
    has_to_write_pid_file = true;
    if (optarg)
      pid_file_path = optarg;
  } else if (option == "log-file")
  {
    if (optarg)
      log_file_path = optarg;
  } else if (option == "debug")
  {
    enabled_debug = true;
  }
}

int main(int argc, char * argv[])
{
  // CTRL + C catcher
  signal(SIGINT, handler);

  // Force the nice value to -20 (urgent)
  nice(-20);

  /*
  **  Application options handling with long options support.
  */
  int c;
  const struct option long_options[] =
  {
    {"daemon",    no_argument,       0, 'd'},
    {"version",   no_argument,       0, 'v'},
    {"help",      no_argument,       0, 'h'},
    {"pid-file",  optional_argument, 0, 'p'},
    {"log-file",  required_argument, 0, 'l'},
    {"debug",     no_argument      , 0, 'D'},
    {0,0,0,0}
  };
  int option_index = 0;
  while ((c = getopt_long(argc, argv, "dvhp:l:D", long_options, &option_index)) != -1)
  {
    switch (c)
    {
      case 0:
        do_from_options(long_options[option_index].name, optarg);
        break;
      case 'd':
        do_from_options("daemon", optarg);
        break;
      case 'v':
        do_from_options("version", optarg);
        break;
      case 'h':
        do_from_options("help", optarg);
        break;
      case 'p':
        do_from_options("pid-file", optarg);
        break;
      case 'l':
        do_from_options("log-file", optarg);
        break;
      case 'D':
        do_from_options("debug", optarg);
        break;
      case '?':
        std::cout << std::endl << "To get help execute me with --help" << std::endl;
        exit(1);
        break;
      default:
        printf("?? getopt returned character code 0%o ??\n", c);
    }
  }

  /*
  **  log4cxx configuration
  *
  *  Appending logs to the file /var/log/douane.log
  */
  log4cxx::PatternLayoutPtr pattern = new log4cxx::PatternLayout(
    enabled_debug ? "%d{dd/MM/yyyy HH:mm:ss} | daemon | %5p | [%F::%c:%L]: %m%n" : "%d{dd/MM/yyyy HH:mm:ss} %5p: %m%n"
  );
  log4cxx::FileAppender * fileAppender = new log4cxx::FileAppender(
    log4cxx::LayoutPtr(pattern),
    log_file_path
  );
  log4cxx::helpers::Pool pool;
  fileAppender->activateOptions(pool);
  log4cxx::BasicConfigurator::configure(log4cxx::AppenderPtr(fileAppender));
  log4cxx::Logger::getRootLogger()->setLevel(enabled_debug ? log4cxx::Level::getDebug() : log4cxx::Level::getInfo());
  /*
  **
  */

  try {
    // Daemonize the application if --daemon argument is passed
    if (has_to_daemonize)
    {
      do_daemonize();
      LOG4CXX_INFO(logger, "A daemon process has been created");
    }

    if (has_to_write_pid_file)
    {
      do_pidfile(pid_file_path);
      LOG4CXX_INFO(logger, "A pid file with PID " << getpid() << " is created at " << pid_file_path);
    }

    LOG4CXX_INFO(logger, "The log file is " << log_file_path);

    if (enabled_debug)
      LOG4CXX_DEBUG(logger, "The debug mode is enabled");

    /*
    ** ~~~~ Global class initializations ~~~~
    */
    LOG4CXX_DEBUG(logger, "Initializing DesktopFiles");
    DesktopFiles          desktop_files;

    LOG4CXX_DEBUG(logger, "Initializing RulesManager");
    RulesManager          rules_manager;

    LOG4CXX_DEBUG(logger, "Initializing ProcessesManager");
    ProcessesManager      processes_manager;
    processes_manager.set_desktop_files(&desktop_files);
    netlink_listener.set_processes_manager(&processes_manager);

    LOG4CXX_DEBUG(logger, "Initializing DBusServer");
    DBusServer            dbus_server;
    dbus_server.set_rules_manager(&rules_manager);
    /*
    **/

    /*
    ** ~~~~ Signal connexions ~~~~
    */

    LOG4CXX_DEBUG(logger, "Connecting objects");
    // When NetlinkListener emit connected_to_kernel_module signal then fire RulesManager::push_rules
    netlink_listener.on_connected_to_kernel_module_connect(boost::bind(&RulesManager::push_rules, &rules_manager));

    // When NetlinkMessageHandler emit new_network_activity signal then fire RulesManager::lookup_activity
    NetlinkMessageHandler::on_new_network_activity_connect(boost::bind(&RulesManager::lookup_activity, &rules_manager, _1));

    // When RulesManager emit new_unknown_activity signal then fire the DBusServer signal NewActivityToBeValidated
    rules_manager.on_new_unknown_activity_connect(boost::bind(&DBusServer::signal_new_unknown_activity, &dbus_server, _1));

    // When D-Bus Douane emit new_rule_received signal then fire RulesManager::make_rule_from
    Douane::on_new_rule_received_connect(boost::bind(&RulesManager::make_rule_from, &rules_manager, _1, _2));

    // When RulesManager emit new_network_activity signal then fire NetlinkListener::send_rule
    rules_manager.on_new_rule_created_connect(boost::bind(&NetlinkListener::send_rule, &netlink_listener, _1));

    // When RulesManager emit rule_deleted signal then fire NetlinkListener::delete_rule
    rules_manager.on_rule_deleted_connect(boost::bind(&NetlinkListener::delete_rule, &netlink_listener, _1));
    //
    // TODO: Emit a signal to the external process which will popup a dialog to ignore the activity
    //rules_manager.on_rule_deleted_connect(boost::bind(&GtkQuestionWindow::forget_unknown_application, &douane_external_dialog, _1));

    // Listener send all received activity to the D-Bus server so that it can fire a signal
    NetlinkMessageHandler::on_new_network_activity_connect(boost::bind(&DBusServer::new_network_activity, &dbus_server, _1));
    /*
    **/

    /*
    ** ~~~~ Daemon starting ~~~~
    */
    // D-Bus server runs in a thread
    dbus_server.start();

    /* Connect and listen to the Linux Kernel Module
    **
    *  The NetlinkListener is the core of the daemon.
    *  This means that the following start() method is the method which will runs until the daemon has to die.
    */
    LOG4CXX_DEBUG(logger, "Starting to listen to LKM. Daemon is alive!");
    netlink_listener.start();

    LOG4CXX_DEBUG(logger, "Daemon is dying...");
    return EXIT_SUCCESS;

  } catch(const std::exception &e)
  {
    LOG4CXX_ERROR(logger, e.what());
  } catch (const std::string &e)
  {
    LOG4CXX_ERROR(logger, e);
  } catch(...)
  {
    LOG4CXX_ERROR(logger, "Unknown error occured!");
  }
}
