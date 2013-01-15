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

#ifndef Runnable_H
#define Runnable_H

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "Timestamp.h"

class Runnable
{
  protected:

  Runnable()
  {
  }

  ~Runnable()
  {
  }

  static int setTimer(const T_timestamp timeout);
  static int resetTimer();

  static void handleTimer(int signal);

  static int setValue(char *&recipient, const char *value);

  static void resetValue(char *&recipient)
  {
    delete [] recipient;

    recipient = NULL;
  }

  private:

  typedef struct
  {
    struct sigaction action_;
    struct itimerval value_;
    int              count_;

  } T_runnable_timer;

  static T_runnable_timer *timer_;
};

#endif /* Runnable_H */
