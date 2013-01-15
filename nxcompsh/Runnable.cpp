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

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "Logger.h"
#include "Runnable.h"

Runnable::T_runnable_timer *Runnable::timer_ = NULL;

void Runnable::handleTimer(int signal)
{
  logTrace("Runnable::handleTimer");

  if (signal == SIGALRM)
  {
    if (timer_ == NULL)
    {
      logTest("Runnable::handleTimer", "No timer set in process %d",
                  getpid());

      logError("Runnable::handleTimer", ESET(ENOENT));
    }
    else
    {
      timer_ -> count_++;

      logTest("Runnable::handleTimer", "Alarm received in process %d "
                  "with count %d", getpid(), timer_ -> count_);

      resetTimer();
    }
  }
  #ifdef TEST
  else
  {
    logTest("Runnable::handleTimer", "Unrecognized "
                "signal received in handler");
  }
  #endif
}

int Runnable::setTimer(const T_timestamp timeout)
{
  logTrace("Runnable::setTimer");

  if (timer_ != NULL)
  {
    logTest("Runnable::setTimer", "Timer already running "
                "in process %d with count %d",
                    getpid(), timer_ -> count_);

    logError("Runnable::setTimer", ESET(EPERM));

    return -1;
  }

  timer_ = new T_runnable_timer;

  if (timer_ == NULL)
  {
    logError("Runnable::setTimer::new", ESET(ENOMEM));

    goto RunnableSetTimerError;
  }

  struct sigaction action;

  action.sa_handler = handleTimer;

  //
  // This field doesn't exist on most OSes except
  // Linux. We keep setting the field to NULL to
  // avoid side-effects in the case the field is
  // a value return.
  //

  #if defined(__linux__)

  action.sa_restorer = NULL;

  #endif

  sigemptyset(&action.sa_mask);

  action.sa_flags = 0;

  sigaction(SIGALRM, &action, &timer_ -> action_);

  //
  // Start the timer.
  //

  struct itimerval timer;

  timer.it_interval = timeout;
  timer.it_value    = timeout;

  logTest("Runnable::setTimer", "Setting timer to %d S "
              "and %d Ms", (int) timeout.tv_sec,
                  (int) timeout.tv_usec / 1000);

  if (setitimer(ITIMER_REAL, &timer, &timer_ -> value_) < 0)
  {
    logError("Runnable::setTimer::setitimer", EGET());

    goto RunnableSetTimerError;
  }

  timer_ -> count_ = 0;

  return 1;

RunnableSetTimerError:

  delete timer_;

  timer_ = NULL;

  return -1;
}

int Runnable::resetTimer()
{
  logTrace("Runnable::resetTimer");

  if (timer_ == NULL)
  {
    logTest("Runnable::resetTimer", "No timer running "
                "in process %d", getpid());

    return 0;
  }

  logTest("Runnable::resetTimer", "Resetting timer "
              "with count %d", timer_ -> count_);

  //
  // Restore the old signal mask and timer.
  //

  int result = 1;

  if (setitimer(ITIMER_REAL, &timer_ -> value_, NULL) < 0)
  {
    logError("Runnable::resetTimer::setitimer", EGET());

    result = -1;
  }

  if (sigaction(SIGALRM, &timer_ -> action_, NULL) < 0)
  {
    logError("Runnable::resetTimer::sigaction", EGET());

    result = -1;
  }

  delete timer_;

  timer_ = NULL;

  return result;
}

int Runnable::setValue(char *&recipient, const char *value)
{
  logTrace("Runnable::setValue");

  if (value == NULL)
  {
    logTest("Runnable::setValue", "Can't set a NULL value "
                "for recipient");

    logError("Runnable::setValue", ESET(EINVAL));

    return -1;
  }

  if (recipient != NULL)
  {
    logTest("Runnable::setValue", "Freeing the previous "
                "recipient with value '%s'", recipient);

    logWarning("Runnable::setValue", ESET(EPERM));

    resetValue(recipient);
  }

  recipient = new char[strlen(value) + 1];

  if (recipient == NULL)
  {
    logError("Runnable::setValue::new", ESET(ENOMEM));

    return -1;
  }

  strcpy(recipient, value);

  return 1;
}
