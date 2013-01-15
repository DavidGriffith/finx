/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXCOMPSH, NX protocol compression, NX extensions to this software      */
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

#include <stdlib.h>
#include <string.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "Logger.h"
#include "System.h"

#define NX_SYSTEM_NAME  "NX_SYSTEM"
#define NX_SYSTEM_SET   "NX_SYSTEM="
#define NX_SYSTEM_SIZE  (sizeof("NX_SYSTEM=") - 1)

#define NX_ROOT_NAME    "NX_ROOT"
#define NX_ROOT_SET     "NX_ROOT="
#define NX_ROOT_SIZE    (sizeof("NX_ROOT=") - 1)

#define NX_HOME_NAME    "NX_HOME"
#define NX_HOME_SET     "NX_HOME="
#define NX_HOME_SIZE    (sizeof("NX_HOME=") - 1)

static char _systemDir[1024];
static char _rootDir[1024];
static char _homeDir[1024];

int setSystemDir(const char *path)
{
  logTrace("System::setSystemDir");

  if (strlen(path) > 1023 - NX_SYSTEM_SIZE)
  {
    logError("System::setSystemDir", ESET(EINVAL));

    return -1;
  }

  strcpy(_systemDir, NX_SYSTEM_SET);
  strcat(_systemDir, path);

  putenv(_systemDir);

  logTest("System::setSystemDir", "Set NX system "
              "directory to '%s'", _systemDir +
                  NX_SYSTEM_SIZE);

  return 1;
}

int setRootDir(const char *path)
{
  logTrace("System::setRootDir");

  if (strlen(path) > 1023 - NX_ROOT_SIZE)
  {
    logError("System::setRootDir", ESET(EINVAL));

    return -1;
  }

  strcpy(_rootDir, NX_ROOT_SET);
  strcat(_rootDir, path);

  putenv(_rootDir);

  logTest("System::setRootDir", "Set NX root "
              "directory to '%s'", _rootDir +
                  NX_ROOT_SIZE);

  return 1;
}

extern int setHomeDir(const char *path)
{
  logTrace("System::setHomeDir");

  if (strlen(path) > 1023 - NX_HOME_SIZE)
  {
    logError("System::setHomeDir", ESET(EINVAL));

    return -1;
  }

  strcpy(_homeDir, NX_HOME_SET);
  strcat(_homeDir, path);

  putenv(_homeDir);

  logTest("System::setHomeDir", "Set NX home "
              "directory to '%s'", _homeDir +
                  NX_HOME_SIZE);

  return 1;
}

const char *getSystemDir()
{
  logTrace("System::getSystemDir");

  if (*_systemDir != '\0')
  {
    return _systemDir + NX_SYSTEM_SIZE;
  }

  const char *path = getenv(NX_SYSTEM_NAME);

  if (path == NULL)
  {
    return NULL;
  }

  setSystemDir(path);

  logTest("System::getSystemDir", "Got NX system "
              "directory '%s'", _systemDir +
                  NX_SYSTEM_SIZE);

  return _systemDir + NX_SYSTEM_SIZE;
}

//
// getRootDir returns the Root dir read from NX_ROOT environment variable
// if getRootDir is not set we guess the root dir '.nx' being relative to
// user's home directory
//

const char *getRootDir()
{
  logTrace("System::getRootDir");

  if (*_rootDir != '\0')
  {
    return _rootDir + NX_ROOT_SIZE;
  }

  const char *path = getenv(NX_ROOT_NAME);

  if (path == NULL)
  {
    return NULL;
  }

  setRootDir(path);

  logTest("System::getRootDir", "Got NX root "
              "directory '%s'", _rootDir +
                  NX_ROOT_SIZE);

  return _rootDir + NX_ROOT_SIZE;
}

const char *getHomeDir()
{
  logTrace("System::getHomeDir");

  if (*_homeDir != '\0')
  {
    return _homeDir + NX_HOME_SIZE;
  }

  const char *path = getenv(NX_HOME_NAME);

  if (path == NULL)
  {
    return NULL;
  }

  setHomeDir(path);

  logTest("System::getHomeDir", "Got NX home "
              "directory '%s'", _homeDir +
                  NX_HOME_SIZE);

  return _homeDir + NX_HOME_SIZE;
}
