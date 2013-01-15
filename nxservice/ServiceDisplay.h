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

#ifndef SERVICEDISPLAY_H
#define SERVICEDISPLAY_H

#include "Service.h"

#ifdef __CYGWIN32__
#include <windows.h>
#endif

#define DISPLAY_BIN "NXWin.exe"

class ServiceDisplay: public Service
{
public:
  ServiceDisplay(const char* path, int argc, char** argv, int nolisten);

  virtual ~ServiceDisplay();
  int getFreeDisplay();

  int startXApple();

  int checkSocketFile();
  virtual int start();
  virtual int checkStart();
  char *getRegistryKey(const char *key, const char *value, HKEY hKey);
 protected:
  const char* name_;
  const char* resolution_;
  int multiwindow_;
  int multiplemonitors_;
  int fullscreen_;
  char* fontserver_;
  char* serverpath_;
  char* authpath_;
  char* display_;
  #ifdef __CYGWIN32__ 
  char* grabKeyboard_;
  int disableDirectDraw_;
  #endif
  int nolisten_;
};

#endif
