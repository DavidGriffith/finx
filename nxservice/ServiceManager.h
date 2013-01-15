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

#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "Service.h"

class ServiceManager
{
public:
  ServiceManager(const char* path);
  virtual ~ServiceManager();

  int list(const char* type);
  int stop(const char* type, int, char**);
  int cleanupDirectories();
protected:
  char root_[DEFAULT_STRING_LENGTH];
};

#endif
