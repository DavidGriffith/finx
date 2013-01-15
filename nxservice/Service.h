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

#ifndef SERVICE_H
#define SERVICE_H

#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#ifndef __CYGWIN32__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define DEFAULT_STRING_LENGTH 512
#define MAX_ARGUMENTS 128

//
// The AF_UNIX socket files have a system
// limit of 108 characters. The service
// device directory can't be longer of 94
// characters because:
//   94 + "/.X11-unix/X0" + '\0' = 108
//

#define MAX_SERVICE_DIR_LENGTH 94


enum ServiceState {
  RUNNING,
  STOPPED
};

enum ServiceType {
  SERVICE_UNKNOWN,
  SERVICE_DISPLAY,
  SERVICE_AUDIO
};

class Service
{
public:
  Service(const char *path, const char *type);
  virtual ~Service();

  //
  // isRunning
  // returns true if the process is still running and false otherwise
  //
  bool isRunning();

  virtual int start();
  virtual int stop();
  virtual int load();
  virtual int renameTerminated();
  virtual int renameFailed();

  int getPid();
  const char* getType();
  virtual const char* getID();

  int logCheck(const char *file, const char *search);
  void checkServiceStart(void);
  virtual int checkStart(void);
  virtual int getTimeout(void);

protected:
  virtual int renameDirectory(const char*);
  virtual int writePidFile();
  virtual int writeOptionsFile();
  virtual int writeCommandFile();
  virtual int writeTypeFile();
  virtual int writePortFile();

  void addArgument(const char *arg);

  //
  // used for running the program when everything is set
  //
  virtual int run();
  virtual int setupPaths(const char* path);
  unsigned int getFreePort(unsigned int startingPort = 0);

  int argumentsNumber_;
  const char *arguments_[MAX_ARGUMENTS];
  char executablePath_[DEFAULT_STRING_LENGTH];

  //
  // i could use the char* here but i want to make them init
  // in constructor and i don't want them to be NULL
  //

  char rootPath_[DEFAULT_STRING_LENGTH];
  char typePath_[DEFAULT_STRING_LENGTH];
  char optionsPath_[DEFAULT_STRING_LENGTH];
  char commandPath_[DEFAULT_STRING_LENGTH];
  char sessionPath_[DEFAULT_STRING_LENGTH];
  char pidPath_[DEFAULT_STRING_LENGTH];
  char portPath_[DEFAULT_STRING_LENGTH];
  char winlogPath_[DEFAULT_STRING_LENGTH];

  int waitFork();

  char type_[DEFAULT_STRING_LENGTH];
  pid_t pid_;
  int   port_;
  ServiceState state_;
  int   timeout_;
};

#endif
