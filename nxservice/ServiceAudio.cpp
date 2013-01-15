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
#include <System.h>
#include "ServiceAudio.h"

#include <string.h>
#include <stdio.h>

ServiceAudio::ServiceAudio(const char* path):
  Service(path, "audio")
{
  timeout_ = 10;
}

int ServiceAudio::start()
{
  const char *systemPath = getSystemDir();
  char s_port[DEFAULT_STRING_LENGTH];

  //
  // FIXME: we do not anything in case of failure
  //
  writeTypeFile();

  if (systemPath == NULL)
  {
    logUser("Error: Cannot find NX system path. Exiting.");
    return -1;
  }

  if ((strlen(systemPath) + strlen("/bin/") + strlen(AUDIO_BIN) + 1) > DEFAULT_STRING_LENGTH)
  {
    logUser("Error: nxesd path too long. Exiting.");
    return -1;
  }

  strncpy(executablePath_, systemPath, DEFAULT_STRING_LENGTH);
  strncat(executablePath_, "/bin/", DEFAULT_STRING_LENGTH);
  strncat(executablePath_, AUDIO_BIN, DEFAULT_STRING_LENGTH);

  port_ = getFreePort(6000);

  if (port_ == 0)
  {
    logUser("Error: Cannot find free port. Exiting.");
    return -1;
  }

  logUser("Info: Using port: %d", port_);

  writePortFile();

  snprintf(s_port, DEFAULT_STRING_LENGTH, "%d", port_);

  arguments_[0] = executablePath_;
  arguments_[1] = "-tcp";
  arguments_[2] = "-nobeeps";
  arguments_[3] = "-port";
  arguments_[4] = s_port;
  arguments_[5] = "-bind";
  arguments_[6] = "127.0.0.1";
  arguments_[7] = "-as";
  arguments_[8] = "10";

  run();

  return 0;
}

int ServiceAudio::checkStart()
{
  //
  // FIXME: We have not a rule to check if the nxesd
  // service is running...
  //

  return 1;
}

