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

#ifndef SERVICEAUDIO_H
#define SERVICEAUDIO_H

#include "Service.h"

#define AUDIO_BIN "nxesd"

class ServiceAudio: public Service
{
public:
  ServiceAudio(const char* path);
  virtual int start();
  virtual int checkStart();
};

#endif
