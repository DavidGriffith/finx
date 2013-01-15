/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2005, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXKILL, NX protocol compression and NX extensions to this software     */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/

#include "Control.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>

/*
 * Apple (OS X) and Sun systems declare getopt in unistd.h,
 * other systems (Linux) use getopt.h
 *
 */

#if defined ( __APPLE__ ) || ( defined (__SVR4) && defined(__sun) )
#include <unistd.h>
#else
#include "getopt.h"
#endif

#ifdef __CYGWIN32__
extern "C" pid_t cygwin_winpid_to_pid (int winpid);
#endif

#define INDEX_LIST    0
#define INDEX_KILL    1
#define INDEX_ID      2
#define INDEX_PID     3
#define INDEX_HARD    4
#define INDEX_SIGNAL  5
#define INDEX_HELP    6
#define INDEX_VERSION 7
#define INDEX_ENVIRONMENT 8
#define INDEX_HANDLE  9

//
// Options Allowed in nxkill application
//

struct option_t
{
  char *name;
  int param;
  int opt1;
  int opt2;
};

/*
static struct option_t OPTIONS[] = {
  {"list", 0, 0, 0},
  {"kill", 0, 0, 0},
  {"id", 1, 0, 0},
  {"pid", 1, 0, 0},
  {"hard", 0, 0, 0},
  {"signal",1, 0, 0},
  {"help", 0, 0, 0},
  {"version", 0, 0, 0},
  {"environment", 0, 0, 0},
  {"handle", 1, 0, 0},
  {0, 0, 0, 0}
};
*/

#undef DEBUG
#undef TEST
#undef WARNING

Control::Control()

  : listMode_(false), killMode_(false), killHard_(false),
    environmentMode_(false), id_(NULL), pid_(-1),
    signal_(SIGTERM),nxDirPath_(NULL)
{

}

Control::~Control()
{
  if (id_ != NULL) delete(id_);
}

void Control::printHelp()
{
  printf("Usage: nxkill [OPTIONS]\n\n");
  printf("--list [--id SESSIONID]\n");
  printf("  If given without a further parameter, it lists the running\n");
  printf("  sessions. If given with a session id, it lists the PIDs of the\n");
  printf("  processes that are run as part of the session.\n\n");
  printf("  Example:\n\n");
  printf("  # nxkill --list\n");
  printf("  S test00 1016 6C84D04D743CEB0A71D6EF9AE4B967E3\n");
  printf("  C test00 1016 6C84D04D743CEB0A71D6EF9AE4B967E3\n");
  printf("  S saturno.medialogic.it 1024 812312312EAECE1231123123210000\n\n");
  printf("  Or:\n\n");
  printf("  # nxkill --list --id 6C84D04D743CEB0A71D6EF9AE4B967E3\n");
  printf("  P 12344\n");
  printf("  X 3211\n\n");
  printf("  P stands for proxy, X for X server.\n\n");
  printf("--id\n");
  printf("  Allows for giving the ID of the session on which to execute the\n");
  printf("  command.\n\n");
  printf("--kill\n\n");
  printf("  Kills the process or the session with the default signal.\n\n");
  printf("  Example:\n\n");
  printf("  # nxkill --kill --pid 123321\n");
  printf("  # nxkill --kill --id 6C84D04D743CEB0A71D6EF9AE4B967E3\n\n");
  printf("--pid\n\n");
  printf("  Allows for giving the PID of the process.\n\n");
  printf("--hard\n\n");
  printf("  If given in a --kill, kills the process or the session hard,\n");
  printf("  trying its best to ensure that the target is terminated before\n");
  printf("  giving up. If it can't, times out after 5 retries and 20 secs.\n");
  printf("  and prints an error.\n\n");
  printf("--signal\n\n");
  printf("  Specify the signal by number.\n\n");
  printf("  Example:\n\n");
  printf("  # nxkill --kill --pid 1234 --signal 2\n\n");
  printf("--help\n\n");
  printf("  Display this help end exit.\n\n");
  printf("--version\n\n");
  printf("  Print version and exit.\n\n");
}

void Control::printVersion()
{
  printf("NXKILL - Version %s\n",VERSION);
}

const char* getArg(int &argi, int argc, const char **argv)
{
  //
  // Skip "-" and flag character.
  //

  argi++;

  if (argi > argc)
  {
    return NULL;
  }

  const char *arg = argv[argi];

  return arg;

/*
  const char *arg = argv[argi] + 2;

  if (*arg == 0)
  {
    if (argi + 1 == argc)
    {
      return NULL;
    }
    else
    {
      argi++;

      return (*argv[argi] == '-' ? NULL : argv[argi]);
    }
  }
  else
  {
    return (*arg == '-' ? NULL : arg);
  }
*/
}

int Control::parseCommandLineOptions(int argc,const char** argv)
{
  #ifdef TEST

  printf("Control::parseCommandLineOptions argc is %d\n",argc);

  for (int argi = 0; argi < argc; argi++)
  {
    printf("Control::parseCommandLineOptions argv[%d] is '%s'\n", argi, argv[argi]);
  }

  #endif

  for (int argi = 1; argi < argc; argi++)
  {
    const char* nextArg = argv[argi];

    if (strncmp(nextArg, "--version", strlen("--version")) == 0)
    {
      printVersion();
      return 1;
    }
    else if (strncmp(nextArg, "--list", strlen("--list")) == 0)
    {
      listMode_ = true;
    }
    else if (strncmp(nextArg, "--kill", strlen("--kill")) == 0)
    {
      killMode_ = true;
    }
    else if (strncmp(nextArg, "--id", strlen("--id")) == 0)
    {
      const char *tmp = NULL;

      if ((listMode_ || killMode_) && (tmp = getArg(argi, argc, argv)))
      {
        id_ = (char *)strdup(tmp);
      }
      else
      {
        return -1;
      }

      break;
    }
    else if (strncmp(nextArg, "--pid", strlen("--pid")) == 0)
    {
      const char *tmp = NULL;

      if ((listMode_ || killMode_) && (tmp = getArg(argi, argc, argv)))
      {
        //
        // FIXME: problem when passing string instead of number
        //        atoi returns 0
        //
        pid_ = atoi(tmp);
      }
      else
      {
        return -1;
      }
    }
    else if (strncmp(nextArg, "--hard", strlen("--hard")) == 0)
    {
      killHard_ = true;
    }
    else if (strncmp(nextArg, "--signal", strlen("--signal")) == 0)
    {
      const char *tmp = NULL;

      if (killMode_ && (tmp = getArg(argi, argc, argv)))
      {
        signal_ = atoi(tmp);
      }
      else
      {
        return -1;
      }
    }
    else if (strncmp(nextArg, "--help", strlen("--help")) == 0)
    {
      printHelp();
      return 1;
    }
    else if (strncmp(nextArg, "--environment", strlen("--environment")) == 0)
    {
      environmentMode_ = true;
    }

    #ifdef __CYGWIN32__
    else if (strncmp(nextArg, "--handle", strlen("--handle")) == 0)
    {
      const char *tmp = NULL;

      handleMode_ = true;

      if (tmp = getArg(argi, argc, argv))
      {
        pid_ = cygwin_winpid_to_pid(atoi(tmp));
      }
      else
      {
        return -1;
      }
    }
    #endif

    else
    {
      printf("Error: Uknown option '%s'\n", nextArg);
      printHelp();
      return -1;
    }
  }

  //
  // When we are in KILL mode we have to have ID or PID
  // specified (both is wrong).
  //

  if ((killMode_ && !((id_ == NULL) ^ (pid_ == -1))))
  {
    printf("ERROR: You should specify PID or ID when killing.\n");
    return -1;
  }


  if ((homePath_ = getenv("NX_HOME")) != NULL)
  {

    nxDirPath_ = homePath_;

    DIR *dir = opendir(homePath_);

    if (dir == NULL)
    {
      printf("Error: Cannot open directory pointed by NX_HOME = '%s'. Exiting.\n",nxDirPath_);
      return -1;
    }

    closedir(dir);

  }
  else if ((homePath_ = getenv("HOME")) != NULL)
  {
    nxDirPath_ = new char[strlen(homePath_) + strlen("/.nx") + 1];

    strcpy(nxDirPath_,homePath_);
    strcat(nxDirPath_,"/.nx");

    DIR *dir = opendir(nxDirPath_);

    if (dir == NULL)
    {
      printf("Error: Cannot open NX directory '%s'. Exiting.\n",nxDirPath_);
      return -1;
    }

    closedir(dir);
  }
  else
  {
    printf("ERROR: NX_HOME or HOME variable are not set. Exiting.\n");
    return -1;
  }

  #ifdef DEBUG
  printf("Debug: nxdir = [%s].\n",nxDirPath_);
  #endif

  return 0;
}


bool Control::isListMode() const
{
  return listMode_;
}

bool Control::isEnvironmentMode() const
{
  return environmentMode_;
}

bool Control::isKillMode() const
{
  return killMode_;
}

bool Control::isKillHard() const
{
  return killHard_;
}

bool Control::isHandleMode() const
{
  return handleMode_;
}

char* Control::getSessionID()
{
  return id_;
}

int Control::getPid() const
{
  return pid_;
}

int Control::getSignal() const
{
  return signal_;
}

char* Control::getDotNXDirPath()
{
  return nxDirPath_;
}

char* Control::getHomePath()
{
  return homePath_;
}
