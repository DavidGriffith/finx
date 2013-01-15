/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2006, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXSERVICE, NX protocol compression and NX extensions to this software  */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rights reserved.                                                   */
/*                                                                        */
/**************************************************************************/

#include <System.h>
#include <Logger.h>

#ifndef __CYGWIN32__
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/types.h>
#else
#include <windows.h>
#include <winsock2.h>
#endif

#include <sys/wait.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include "Service.h"

#if defined(__APPLE__) && !defined(__i386__)
typedef int socklen_t;
#endif

#define WARNING
#undef  TEST
#define DEBUG

//
// On Cygwin, retry n times waiting for
// a second afer each attempt.
//

#define RETRY_FORK_LIMIT        30
#define RETRY_FORK_INTERVAL     1000

//
// Wait between multiple checks of the
// service's status.
//

#define SERVICE_START_INTERVAL  200

Service::Service(const char *filepath, const char *type)
{
  strncpy(type_, type, DEFAULT_STRING_LENGTH);
  setupPaths(filepath);
  pid_ = 0;
  state_ = STOPPED;
  port_ = 0;
  argumentsNumber_ = 0;
  timeout_ = 15;
}

Service::~Service()
{
}

int Service::setupPaths(const char* filepath)
{
  memset(arguments_, '\0', MAX_ARGUMENTS * sizeof(char *));
  memset(executablePath_, '\0', DEFAULT_STRING_LENGTH);
  memset(rootPath_, '\0', DEFAULT_STRING_LENGTH);
  memset(typePath_, '\0', DEFAULT_STRING_LENGTH);
  memset(optionsPath_, '\0', DEFAULT_STRING_LENGTH);
  memset(sessionPath_, '\0', DEFAULT_STRING_LENGTH);
  memset(pidPath_, '\0', DEFAULT_STRING_LENGTH);
  memset(commandPath_, '\0', DEFAULT_STRING_LENGTH);
  memset(portPath_, '\0', DEFAULT_STRING_LENGTH);
  memset(winlogPath_, '\0', DEFAULT_STRING_LENGTH);

  #ifdef DEBUG
  logUser("Service::setupPaths filepath='%s'", filepath);
  #endif

  strncpy(rootPath_, filepath, DEFAULT_STRING_LENGTH);

  strncpy(typePath_, rootPath_, DEFAULT_STRING_LENGTH);
  strncat(typePath_, "/type", DEFAULT_STRING_LENGTH - strlen(rootPath_) -1);

  strncpy(optionsPath_, rootPath_, DEFAULT_STRING_LENGTH);
  strncat(optionsPath_, "/options", DEFAULT_STRING_LENGTH - strlen(rootPath_) -1);

  strncpy(sessionPath_, rootPath_, DEFAULT_STRING_LENGTH);
  strncat(sessionPath_, "/session", DEFAULT_STRING_LENGTH - strlen(rootPath_) -1);

  strncpy(pidPath_, rootPath_, DEFAULT_STRING_LENGTH);
  strncat(pidPath_, "/pid", DEFAULT_STRING_LENGTH - strlen(rootPath_) -1);

  strncpy(commandPath_, rootPath_, DEFAULT_STRING_LENGTH);
  strncat(commandPath_, "/command", DEFAULT_STRING_LENGTH - strlen(rootPath_) -1);

  strncpy(portPath_, rootPath_, DEFAULT_STRING_LENGTH);
  strncat(portPath_, "/port", DEFAULT_STRING_LENGTH - strlen(rootPath_) -1);

  strncpy(winlogPath_, rootPath_, DEFAULT_STRING_LENGTH);
  strncat(winlogPath_, "/winlog", DEFAULT_STRING_LENGTH - strlen(rootPath_) -1);

  #ifdef DEBUG
  logUser("Service::setupPaths rootPath='%s'", rootPath_);
  #endif

  return 0;
}


bool Service::isRunning()
{
  if(/*state_ == STOPPED ||*/ pid_ <= 0)
  {
    return false;
  }
  else
  {
    /*
     * if state is running we need to be sure that
     * it didn't die in the meanwhile
     */
    if (kill(pid_, SIGCONT) == 0)
    {
      return true;
    }
    else
    {
      state_ = STOPPED;
      /* pid_ = 0; */
      return false;
    }
  }
}

int Service::start()
{
  return 0;
}

#ifndef __CYGWIN32__
unsigned int Service::getFreePort(unsigned int startingPort)
{
  int fd = 0;

  sockaddr_in address;

  memset(&address, '\0', sizeof(address));

  if (startingPort <= 0)
  {
    startingPort = 1025;
  }

  fd = socket(PF_INET, SOCK_STREAM, PF_UNSPEC);

  address.sin_family = AF_INET;

  address.sin_addr.s_addr = htonl(INADDR_ANY);


  for (unsigned int port = startingPort; port < 65000; port++)
  {
    address.sin_port = htons(port);

    if (bind(fd, (sockaddr *)&address, sizeof(address)) == 0)
    {
      //
      // it means that everything is ok
      //
      close(fd);
      return port;
    }
  }

  close(fd);

  //
  // in case of problems
  //
  return 0;
}

#else

unsigned int Service::getFreePort(unsigned int startingPort)
{
  unsigned int port = 0;
  unsigned int searchPort = startingPort;
  WSADATA info;

  if (WSAStartup(MAKEWORD(1, 1), &info) != 0)
  {
    return (port = 0);
  }

  SOCKET s;
  struct sockaddr_in sa;
  struct hostent *hp;

  memset(&sa, 0, sizeof(struct sockaddr_in));
  hp = gethostbyname("localhost");

  if (hp == NULL)
  {
    ::WSACleanup();
    return (port = 0);
  }

  for (;searchPort < 65000; searchPort++)
  {
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons(searchPort);
    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s == INVALID_SOCKET)
    {
      port = 0;

      #ifdef WARNING
      logUser("Service::getFreePort: invalid socket.");
      #endif

      break;
    }

    if (bind(s, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
      closesocket(s);
      port = 0;
      continue;
    }
    port = searchPort;
    closesocket(s);
    break;
  }
  ::WSACleanup();

  #ifdef WARNING
  logUser("Service::getFreePort: found port[%d]", port);
  #endif

  return port;
}
#endif

int Service::writePidFile()
{
  FILE *filePtr = fopen(pidPath_, "w");

  if (filePtr == NULL)
  {
    return -1;
  }

  fprintf(filePtr, "%d", pid_);
  fclose(filePtr);

  return 0;
}

int Service::writePortFile()
{
  if (port_ != 0)
  {
    FILE *filePtr = fopen(portPath_, "w");

    if (filePtr == NULL)
    {
      return -1;
    }

    fprintf(filePtr, "%d", port_);
    fclose(filePtr);
  }
  return 0;
}


int Service::writeCommandFile()
{
  FILE *filePtr = fopen(commandPath_, "w");

  if (filePtr == NULL)
  {
    return -1;
  }

  int n = 0;

  while (arguments_[n])
  {
    fprintf(filePtr, "%s", arguments_[n++]);

    if (arguments_[n])
    {
      fprintf(filePtr, "%s"," ");
    }
  }
  fclose(filePtr);
  return 0;
}


int Service::writeOptionsFile()
{
  FILE *filePtr = fopen(optionsPath_, "w");

  if (filePtr == NULL)
  {
    return -1;
  }

  //
  // we don't want to write the program name
  //
  int n = 1;

  while (arguments_[n])
  {
    bool paramName = false;
    const char *str = arguments_[n++];

    if (*str == '-')
    {
      paramName = true;
      str++;
    }
    else
    {
      paramName = false;
    }

    //
    // in case of double dash
    //
    if (*str == '-')
    {
      str++;
    }

    if (!*str)
    {
      continue;
    }

    fprintf(filePtr, "%s", str);

    if (arguments_[n])
    {
      if (*arguments_[n] == '-')
      {
        if (paramName)
        {
          fprintf(filePtr, "%s", "=1,");
        }
        else
        {
          fprintf(filePtr, "%s", ",");
        }
      }
      else
      {
        fprintf(filePtr, "%s", "=");
      }
    }
  }

  fclose(filePtr);

  return 0;
}

int Service::writeTypeFile()
{
  FILE *filePtr = fopen(typePath_, "w");

  if (filePtr == NULL)
  {
    return -1;
  }

  fprintf(filePtr, "%s", type_);
  fclose(filePtr);

  return 0;
}

int Service::waitFork()
{
  int limit   = RETRY_FORK_LIMIT;
  int timeout = RETRY_FORK_INTERVAL;

  int pid;

  for (int i = 0; i < limit; i++)
  {
    //
    // It could optionally try again only if the
    // error code is 11, 'Resource temporarily
    // unavailable'.
    //

    if ((pid = fork()) >= 0)
    {
      break;
    }
    else if (i < limit - 1)
    {
      #ifdef TEST
      logUser("Service::waitFork:fork: Function "
                     "fork failed. Retrying...");
      #endif

      usleep(timeout * 1000);
    }
  }

  return pid;
}

int Service::run()
{
  writeCommandFile();
  writeOptionsFile();

  logUser("Info: Starting daemon: '%s'", getID());

  #ifdef __CYGWIN32__
 
  if ((pid_ = waitFork()) != 0)

  #else

  if ((pid_ = fork()) != 0)

  #endif

  {
    if (pid_ < 0)
    {
      logUser("Error: Fork failed.");
    }
    else
    {
      #ifdef DEBUG
      logUser("Info: Created process with pid=%d", pid_);
      #endif
    }

    writePidFile();

    //
    // The parent waits for a signal or a log
    // to know if its child (the service) starts
    // successfully.
    //

    if (pid_ > 0)
    {
      checkServiceStart();
    }

    return pid_;
  }

  #ifdef DEBUG
  logUser("Info: Executing child with pid=%d and ppid=%d", getpid(), getppid());
  #endif

  if (execvp(executablePath_, (char *const *) arguments_) == -1)
  {
    logUser("Error: Cannot execute '%s'. Error: '%s'", executablePath_, strerror(errno));
    exit(-1);
  }

   /*
    * checkServiceStart() get rid of this.
    *
    exit(0);
    */

  //
  // just to shut up the compiler
  //
  return 0;
}

int Service::load()
{
  //
  // read file type
  //
  FILE *filePtr = fopen(typePath_, "r");

  if (filePtr == NULL)
  {
    return -1;
  }

  fscanf(filePtr, "%s", type_);
  fclose(filePtr);

  //
  // read pid file
  //

  filePtr = fopen(pidPath_, "r");

  if (filePtr == NULL)
  {
    return -1;
  }

  int pid = 0;

  fscanf(filePtr, "%d", &pid);
  pid_ = pid;
  fclose(filePtr);

  return 0;
}

int Service::getPid()
{
  return pid_;
}

const char* Service::getType()
{
  return type_;
}

int Service::stop()
{
  int ret;

  if (pid_ == -1)
  {
    #ifdef WARNING
    logUser("Service::stop: WARNING! No process running");
    #endif

    //
    // Let's say kill() succeded.
    //

    ret = 0;
  }
  else
  {
    ret = kill(pid_, SIGTERM);
  }

  if (ret == 0)
  {
    renameDirectory("T-");
  }
  else
  {
    renameDirectory("F-");
  }

  return (ret == 0);
}

int Service::renameDirectory(const char* prefix)
{
  char *slash = NULL;

  char path[DEFAULT_STRING_LENGTH];

  strncpy(path, rootPath_, DEFAULT_STRING_LENGTH);

  slash = strrchr(path, '/');

  if (slash == NULL)
  {
    return -1;
  }

  char *name = new char[strlen(slash) + strlen(prefix) + 1];

  if (name == NULL)
  {
    return -1;
  }

  strcpy(name, prefix);

  //
  // this is safe in the worst case slash + 1 will be '\0'
  //
  strcat(name, slash + 1);

  #ifdef DEBUG
  logUser("Info: renameDirectory: name='%s'",name);
  #endif

  if (strlen(path) + strlen(prefix) > DEFAULT_STRING_LENGTH)
  {
    return -1;
  }

  strcpy(slash + 1, name);

  //
  // FIXME: Sometimes we try to rename the session directories
  // before a service closes its log file. Should we retry
  // until the rename succeded?
  //

  if (rename((const char*)rootPath_, (const char*)path))
  {
    logUser("Error: Cannot rename '%s' to '%s'. Error is '%s'", rootPath_, path, strerror(errno));
    return -1;
  }
  else
  {
    return -1;
  }

  setupPaths(path);

  delete [] name;

  return 0;
}

int Service::renameTerminated()
{
  return renameDirectory("T-");
}

int Service::renameFailed()
{
  return renameDirectory("F-");
}

void Service::addArgument(const char *arg)
{
   if (argumentsNumber_ < MAX_ARGUMENTS)
   {
      logUser("Service::addArgument[%d][%s]", argumentsNumber_, arg);
      arguments_[argumentsNumber_++] = arg;
   }
   else
   {
      logUser("Service::addArgument: Error cannot add: [%d][%s]", argumentsNumber_, arg);
   }
}

const char* Service::getID()
{
  char *id = strrchr(rootPath_, '-');

  if (id != NULL)
  {
    id++;
  }

  return id;
}

int Service::logCheck(const char *file, const char *search)
{
  FILE *fp;
  char line[DEFAULT_STRING_LENGTH];
  int found;

  if (file == NULL)
  {
    #ifdef WARNING
    logUser("Service::logCheck: WARNING! No file specified.");
    #endif

    return 0;
  }

  fp = fopen(file, "r");

  if (fp == NULL)
  {
    #ifdef WARNING
    logUser("Service::logCheck: WARNING! Cannot open the file: (error %d) %s", EGET(), ESTR());
    #endif

    return 0;
  }

  //
  // TODO: This function could be optimized using
  // the search stream direction.
  //

  found = 0;

  while (!feof(fp))
  {
    fgets(line, DEFAULT_STRING_LENGTH-1, fp);

    #ifdef TEST
    logUser("Service::logCheck: Going to examine line [%s].", line);
    #endif

    if (strstr(line, search) != NULL)
    {
      found = 1;
      break;
    }
  }

  fclose(fp);

  return (found);
}

void Service::checkServiceStart(void)
{
  int status = 0;
  int wpid;
  int options = WNOHANG | WUNTRACED;

  static int timeout;

  //
  // The child is the service launched by nxservice.
  // Each second we check for the child exit status.
  // If the child doesn't return, we use its logs to
  // retrieve the status.
  //

  #ifdef DEBUG
  logUser("Service::checkServiceStart: going to check the '%s' service status", getType());
  #endif

  for (;;)
  {
    #ifdef TEST
    logUser("Service::checkServiceStart: waiting for child pid %d.", getPid());
    #endif

    wpid = waitpid(getPid(), &status, options);

    if (wpid == -1)
    {
      #ifdef WARNING
      logUser("WARNING! Waitpid returns error: %d.", EGET());
      #endif

      //
      // "The process specified does not exist or is not
      // a child of the calling process."
      // We don't know why this happened, but we can try
      // to return a successful state.
      //

      exit(0);
    }
    else if (wpid == 0)
    {
      //
      // The child's state is not changed. Let's check
      // if there is a reason to believe it is running.
      //

      if (checkStart())
      {
        #ifdef DEBUG
        logUser("Service::checkServiceStart: '%s' successfully starts after waiting "
                    "for %d seconds.", getType(), timeout / 1000);
        #endif

        exit(0);
      }
    }
    else
    {
      //
      // The child's state changes, let's figure out why.
      //

      if (WIFSTOPPED(status))
      {
        #ifdef DEBUG
        logUser("Service::checkServiceStart: '%s' service has been stopped with signal %d "
                    "after waiting for %d seconds.", getType(), WSTOPSIG(status),
                        timeout / 1000);
        #endif

        exit(0);
      }
      else if (WIFEXITED(status))
      {
        #ifdef DEBUG
        logUser("Service::checkServiceStart: '%s' service exits with status %d after "
                    "waiting for %d seconds.", getType(), WEXITSTATUS(status),
                        timeout / 1000);
        #endif

        exit(WEXITSTATUS(status));
      }
      else if (WIFSIGNALED(status))
      {
        #ifdef DEBUG
        logUser("Service::checkServiceStart: '%s' service terminates with signal %d after "
                    "waiting for %d seconds.", getType(), WTERMSIG(status),
                        timeout / 1000);
        #endif

        //
        // FIXME: Do we need to better classify the signals?
        //

        switch (WTERMSIG(status))
        {
          case SIGABRT:
          case SIGSEGV:
          case SIGKILL:
            exit(1);
          default:
            exit(0);
        }
      }
      else
      {
        #ifdef WARNING
        logUser("Service::checkServiceStart: WARNING! Waitpid returns an unknown status: "
                    "%d.", status);
        #endif
      }
    }

    usleep(SERVICE_START_INTERVAL * 1000);

    timeout += SERVICE_START_INTERVAL;

    if (timeout / 1000 >= getTimeout())
    {
      #ifdef WARNING
      logUser("Service::checkServiceStart: WARNING! Assuming the '%s' starts after "
                  "waiting for %d seconds.", getType(), timeout / 1000);
      #endif

      exit(0);
    }
  }
}

int Service::checkStart()
{
  return 1;
}

int Service::getTimeout()
{
  return timeout_;
}

