/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2006, 2011 NoMachine, http://www.nomachine.com.          */
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

#include <MD5.h>
#include <System.h>
#include <Logger.h>
#include <Timestamp.h>
#include "Service.h"
#include "ServiceAudio.h"
#include "ServiceDisplay.h"
#include "ServiceManager.h"

enum Mode {
  UNKNOWN,
  LIST,
  START,
  STOP,
  CLEANUP,
  PROGRAM_VERSION,
  CHECK
};

char * serviceId;

//
// As default we don't let the nxwin
// listen on TCP sockets.
//

int nolisten = AF_INET;

static Mode parseArguments(char*& programName, int argc, char** argv);
static int getService(const char* str);
static char* createServiceDirectory();
static int startService(char* programName, int argc, char** argv);
static int stopService(char* programName, int argc, char** argv);
static int listServices(char* programName);
static int cleanupDirectories();

Mode parseArguments(char*& programName, int argc, char** argv)
{
  char *arg = argv[1];

  programName = NULL;

  serviceId = NULL;

  Mode mode = UNKNOWN;

  if (strcmp(arg,"--list") == 0)
  {
    mode = LIST;
  }
  else if (strcmp(arg, "--start") == 0)
  {
    mode = START;
  }
  else if (strcmp(arg, "--stop") == 0)
  {
    mode = STOP;
  }
  else if (strcmp(arg, "--cleanup") == 0)
  {
    mode = CLEANUP;
  }
  else if (strcmp(arg, "--version") == 0)
  {
    mode = PROGRAM_VERSION;
  }
  else if (strcmp(arg, "--check") == 0)
  {
    mode = CHECK;
  }
  else
  {
    mode = UNKNOWN;
  }

  if (argc > 2)
  {
    programName = strdup(argv[2]);
    logUser("\nInfo: Service name: %s [%s].", programName, strTimestamp());

    if (argc > 4 && strcmp(argv[3], "--id") == 0)
    {
      serviceId = argv[4];
      logUser("Info: Service id: %s.", serviceId);
    }
  }

  return mode;
}

char* createServiceDirectory()
{
  const char *rootPath = getRootDir();

  char hostname[DEFAULT_STRING_LENGTH];

  int serviceDirLength;

  if (rootPath == NULL)
  {
    //
    // exit!
    //
    logUser("Error: Cannot get the root directory.");
    return NULL;
  }

  if (gethostname(hostname, DEFAULT_STRING_LENGTH) == -1)
  {
    //
    // we cannot get hostname, we return - maybe we should just
    // set it localhost?
    //
    logUser("Error: Cannot retrieve the hostname.");
    return NULL;
  }

  #define MD5_LENGTH 16
  char md5[(MD5_LENGTH * 2) + 1];
  memset(md5, '\0', (MD5_LENGTH * 2) + 1);

  if (serviceId == NULL)
  {
    srandom(time(0) * getpid());

    long int a = random();

    char *astr = new char[sizeof(a) + 1];

    strncpy(astr,(char *)&a, sizeof(a));

    md5_state_t *md5_state = new md5_state_t();
    unsigned char *md5_digest = new unsigned char[MD5_LENGTH];
    md5_init(md5_state);
    md5_append(md5_state, (const unsigned char *)astr, sizeof(astr));
    md5_finish(md5_state, md5_digest);

    int di = 0;

    for (di = 0; di < MD5_LENGTH; ++di)
    {
      sprintf(md5 + (di * 2), "%02X", md5_digest[di]);
    }

    *(md5 + (di * 2)) = '\0';

    delete [] md5_state;
    delete [] md5_digest;
    delete [] astr;
  }
  else
  {
    strncpy(md5, serviceId, MD5_LENGTH * 2);
  }

  logUser("Info: Service directory MD5 %s.", md5);

  serviceDirLength = strlen(rootPath) + strlen("/D-") + strlen(hostname) + strlen("-") + strlen(md5) + 1;

  //
  // In no case the service directory path can
  // be longer than DEFAULT_STRING_LENGTH chars.
  //

  if (serviceDirLength > DEFAULT_STRING_LENGTH)
  {
    logUser("PANIC: Service directory name would exceed length of %d characters.", DEFAULT_STRING_LENGTH);

    return NULL;
  }

  //
  // If the service directory path is longer than
  // MAX_SERVICE_DIR_LENGTH, we opened only the TCP
  // socket connection to avoid the creation of a
  // UNIX socket file. Read the comments in 
  // ServiceDisplay::start() about the -nolisten
  // option for more details.
  //

  if (serviceDirLength > MAX_SERVICE_DIR_LENGTH)
  {
    logUser("Warning: Service directory path too long. Listening on AF_INET socket.");

    nolisten = AF_UNIX;
  }

  char *path = new char[strlen(rootPath) + strlen("/D-") + strlen(hostname) + strlen("-") + strlen(md5) + 1];

  //
  // Transform name in a fully qualified name.
  //

  strcpy(path, rootPath);
  strcat(path, "/D-");
  strcat(path, hostname);
  strcat(path, "-");
  strcat(path, md5);

  if (mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR))
  {
    //
    // we cannot create the directory
    //
    int errorNumber = errno;
    logUser("Error: Cannot create directory '%s'",path);
    logUser("Error: because of '%s'", strerror(errorNumber));

    if (errorNumber == EEXIST) 
    {
      logUser("Error: Directory exists already, we can use it!");
    }
    else  
    {    
      delete [] path;
      path = NULL;
    }
  }

  return path;
}

int getService(const char* str)
{
  int ret = SERVICE_UNKNOWN;

  if (str)
  {
    if (strcmp(str, "audio") == 0)
    {
      ret = SERVICE_AUDIO;
    }
    else if (strcmp(str, "display") == 0)
    {
      ret = SERVICE_DISPLAY;
    }
  }
  return ret;
}

int startService(char* programName, int argc, char** argv)
{
  if (programName)
  {
    char *path = createServiceDirectory();
    char newEnv[DEFAULT_STRING_LENGTH];

    //
    // This is a fatal error.
    //

    if (!path)
    {
       return -1;
    }

    logUser("Info: Using directory: '%s'", path);

    sprintf(newEnv, "NX_TEMP=%s", path);

    putenv(newEnv);

    Service *service = NULL;

    switch (getService(programName))
    {
    case SERVICE_AUDIO:
      service = new ServiceAudio(path);
      break;
    case SERVICE_DISPLAY:
      service = new ServiceDisplay(path, argc, argv, nolisten);
      break;
    default:
      logUser("Error: Unknown service.");
      delete [] path;
      return -1;
    }

    if (service)
    {
      if (service->start())
      {
        logUser("Error: Service start failed.");
        delete [] path;
        return -1;
      }
      else
      {
        logUser("Info: Service start.");
      }
    }

    delete [] path;
  }
  else
  {
    logUser("Error: Unknown service.");
    return -1;
  }

  return 0;
}

int stopService(char* programName, int argc, char** argv)
{
  ServiceManager sm(getRootDir());
  int ret = sm.stop(programName, argc, argv);

  return (ret);
}

int listServices(char* programName)
{
  const char *root = getRootDir();
  int ret = 0;

  if (root != NULL)
  {
    ServiceManager sm(root);
    ret = sm.list(programName);
  }
  else
  {
    return -1;
  }

  return (ret);
}

int cleanupDirectories()
{
  ServiceManager sm(getRootDir());
  int ret = sm.cleanupDirectories();

  return (ret);
}

int main(int argc, char** argv)
{
  //
  // Be sure all files are only readable
  // by the user.
  //

  umask(0077);

  char *programName = NULL;

  if (argc > 1)
  {
    switch(parseArguments(programName, argc, argv))
    {
    case LIST:
      if (listServices(programName))
      {
        return 1;
      }
      break;
    case START:
      if (startService(programName, argc, argv))
      {
        return 1;
      }
      break;
    case STOP:
      if (stopService(programName, argc, argv))
      {
        return 1;
      }
      break;
    case CLEANUP:
      if (cleanupDirectories())
      {
        return 1;
      }
      break;
    case CHECK:
      logUser("NXSERVICE - is running");
      break;
    case PROGRAM_VERSION:
      logUser("NXSERVICE - version %s", VERSION);
      break;
    case UNKNOWN:
    default:
      logUser("Error: unknown parameter.");
      return 1;
    }
  }
  else
  {
    logUser("Error: Wrong parameters.");
    return 1;
  }

  return 0;
}
