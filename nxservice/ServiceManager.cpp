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

#include <Logger.h>
#include "ServiceManager.h"

ServiceManager::ServiceManager(const char* path)
{
  memset(root_, '\0', DEFAULT_STRING_LENGTH);

  if (path != NULL)
  {
    strncpy(root_, path, DEFAULT_STRING_LENGTH);
  }
}

ServiceManager::~ServiceManager()
{
}

int ServiceManager::cleanupDirectories()
{
  DIR *rootDir = opendir(root_);

  logUser("Info::ServiceManager cleanup");

  if (rootDir != NULL)
  {
    dirent *dirEntry = NULL;

    while ((dirEntry = readdir(rootDir)) != NULL)
    {
      char *name = dirEntry->d_name;
      if (name != NULL && name[0] != '.' && strncmp(name, "D-", 2) == 0)
      {
        char *path = new char[strlen(root_) + strlen("/") + strlen(name) + 1];
        if (path != NULL)
        {
          strcpy(path, root_);
          strcat(path, "/");
          strcat(path, name);
          Service service(path,"");
          service.load();

          if (!service.isRunning())
          {
            //
            // it was not started or stopped properly and is not running so it means 
            // we should move it as failed.
            //
            service.renameFailed();
          }

          delete [] path;
        }
      }
    }
  }
  else
  {
    logUser("Error: Cannot open root directory '%s'.",root_);
    return -1;
  }

  return 0;
}

int ServiceManager::list(const char *type)
{
  DIR *rootDir = opendir(root_);

  logUser("Info::ServiceManager list '%s'", type);

  if (rootDir != NULL)
  {
    dirent *dirEntry = NULL;

    while ((dirEntry = readdir(rootDir)) != NULL)
    {
      char *name = dirEntry->d_name;
      if (name != NULL && name[0] != '.' && strncmp(name, "D-", 2) == 0)
      {
        char *path = new char[strlen(root_) + strlen("/") + strlen(name) + 1];
        if (path != NULL)
        {
          strcpy(path, root_);
          strcat(path, "/");
          strcat(path, name);
          Service service(path,"");
          service.load();

          if (type == NULL || (type != NULL && strcmp(type, service.getType())==0)
            || strlen(service.getType())==0)// && service.isRunning())
          {
            logUser("%d %s %s is %s", service.getPid(), service.getType(),
                    name, (service.isRunning() ? "running" : "not running"));
          }

          delete [] path;
        }
      }
    }
  }
  else
  {
    logUser("Error: Cannot open root directory '%s'.",root_);
    return -1;
  }

  return 0;
}

int ServiceManager::stop(const char *type, int argc, char** argv)
{
  DIR *rootDir = opendir(root_);
  char * id = NULL;

  logUser("Info::ServiceManager stop '%s'", type);

  //
  // let's check if we have the id for the service
  //

  if (argc > 3  && strcmp(argv[3],"--id") == 0)
  {
    if (argc == 5)
       id = argv[4];

    if (id == NULL || strlen(id) == 0)
    {
       /*
        * we quit as we don't know which process to stop
        */

       logUser("Error: id value empty");
       fflush(NULL);
       return 0;
    }
    else
    {
      logUser("Info: id value '%s'", id);
      fflush(NULL);
    }
  }

  if (rootDir != NULL)
  {
    dirent *dirEntry = NULL;

    while ((dirEntry = readdir(rootDir)) != NULL)
    {
      char *name = dirEntry->d_name;
      if (name != NULL && name[0] != '.' && strncmp(name, "D-", 2) == 0)
      {
        char *path = new char[strlen(root_) + strlen("/") + strlen(name) + 1];

        if (path != NULL)
        {
          strcpy(path, root_);
          strcat(path, "/");
          strcat(path, name);
          Service service(path,"");
          service.load();

          if (type != NULL && strcmp(type, service.getType())==0 && service.isRunning())
          {
	     /*
	      * if id was not specified we stop all the services of one kind
	      *
	      */

            if (!id || (id && strcmp(service.getID(), id) == 0))
            {
              logUser("Info: Stopping pid '%d'.",service.getPid());
              service.stop();
            }
          }

          delete [] path;
        }
      }
    }
  }
  else
  {
    logUser("Error: Cannot open root directory '%s'.", root_);
    return -1;
  }

  return 0;
}
