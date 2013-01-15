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
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "Logger.h"
#include "Dispatcher.h"

Dispatcher::Dispatcher()
{
  logTrace("Dispatcher::Dispatcher");

  FD_ZERO(&readSet_);
  FD_ZERO(&writeSet_);

  resultFds_ = 0;
  limitFd_   = 0;

  selectTs_ = nullTimestamp();

  FD_ZERO(&pooledReadSet_);
  FD_ZERO(&pooledWriteSet_);

  pooledReadFds_  = 0;
  pooledWriteFds_ = 0;

  lowerPooledReadFd_ = 0;
  upperPooledReadFd_ = 0;
  firstPooledReadFd_ = 0;

  lowerPooledWriteFd_ = 0;
  upperPooledWriteFd_ = 0;
  firstPooledWriteFd_ = 0;
}

Dispatcher::~Dispatcher()
{
  logTrace("Dispatcher::~Dispatcher");
}

int Dispatcher::start()
{
  logTrace("Dispatcher::start");

  return 1;
}

int Dispatcher::waitEvent(T_timestamp &timeout)
{
  logTrace("Dispatcher::waitEvent");

  fd_set *pReadSet  = NULL;
  fd_set *pWriteSet = NULL;

  T_timestamp *pSelectTs = NULL;

  if (pooledReadFds_ > 0)
  {
    logTest("Dispatcher::waitEvent", "Waiting for %d read "
                "descriptors", pooledReadFds_);

    memcpy(&readSet_, &pooledReadSet_, sizeof(readSet_));

    pReadSet = &readSet_;
  }

  if (pooledWriteFds_ > 0)
  {
    logTest("Dispatcher::waitEvent", "Waiting for %d write "
                "descriptors", pooledWriteFds_);

    memcpy(&writeSet_, &pooledWriteSet_, sizeof(writeSet_));

    pWriteSet = &writeSet_;
  }

  if (isTimestamp(timeout) == 1)
  {
    logTest("Dispatcher::waitEvent", "Waiting for a timeout of "
                "%d S and %d Ms", (int) timeout.tv_sec,
                    (int) timeout.tv_usec / 1000);

    selectTs_ = timeout;

    pSelectTs = &selectTs_;
  }

  limitFd_ = (upperPooledReadFd_ > upperPooledWriteFd_ ?
                  upperPooledReadFd_ + 1 : upperPooledWriteFd_ + 1);

  logTest("Dispatcher::waitEvent", "Upper limit of awaited descriptor is %d",
              limitFd_);

  resultFds_ = select(limitFd_, pReadSet, pWriteSet, NULL, pSelectTs);

  //
  // Check the result of select.
  //

  if (resultFds_ <= 0)
  {
    if (resultFds_ < 0)
    {
      //
      // Check if the call was interrupted or if any of the
      // managed descriptors has become invalid. This can
      // happen in the X11 code, just before the descriptor
      // is removed from the managed set.
      //

      #ifdef __sun

      if (EGET() == EINTR || EGET() == EBADF ||
              EGET() == EINVAL)

      #else

      if (EGET() == EINTR || EGET() == EBADF)

      #endif

      {
        #ifdef TEST

        if (EGET() == EINTR)
        {
          logTest("Dispatcher::waitEvent", "Select failed "
                      "due to an interrupt");
        }
        else
        { 
          logTest("Dispatcher::waitEvent", "Select failed "
                      "due to an I/O error");

          logWarning("Dispatcher::waitEvent", EGET());
        }

        #endif
      }
      else
      {
        logTest("Dispatcher::waitEvent", "Select failed "
                    "due to an unexpected error");

        logError("Dispatcher::waitEvent", EGET());
      }
    }

    return resultFds_;
  }

  return resultFds_;
}

int Dispatcher::nextReadEvent()
{
  logTrace("Dispatcher::nextReadEvent");

  if (resultFds_ > 0)
  {
    for (int j = (firstPooledReadFd_ + 1 > upperPooledReadFd_ ?
             lowerPooledReadFd_ : firstPooledReadFd_ + 1);
                 ; (j + 1 > upperPooledReadFd_ ?
                     j = lowerPooledReadFd_ : j++))
    {
      logTest("Dispatcher::nextReadEvent", "Looping with first %d "
                  "and current %d", firstPooledReadFd_, j);

      if (FD_ISSET(j, &pooledReadSet_))
      {
        logTest("Dispatcher::nextReadEvent", "Returning descriptor %d", j);

        resultFds_--;

        return j;
      }

      if (j == firstPooledReadFd_)
      {
        firstPooledReadFd_ = (firstPooledReadFd_ + 1 >
                                  upperPooledReadFd_ ? lowerPooledReadFd_ :
                                      firstPooledReadFd_ + 1);
        break;
      }
    }
  }

  logTest("Dispatcher::nextReadEvent", "No descriptor remaining");

  return -1;
}

int Dispatcher::end()
{
  logTrace("Dispatcher::end");

  FD_ZERO(&readSet_);
  FD_ZERO(&writeSet_);

  resultFds_ = 0;
  limitFd_   = 0;

  selectTs_ = nullTimestamp();

  FD_ZERO(&pooledReadSet_);
  FD_ZERO(&pooledWriteSet_);

  pooledReadFds_  = 0;
  pooledWriteFds_ = 0;

  lowerPooledReadFd_ = 0;
  upperPooledReadFd_ = 0;
  firstPooledReadFd_ = 0;

  lowerPooledWriteFd_ = 0;
  upperPooledWriteFd_ = 0;
  firstPooledWriteFd_ = 0;

  return 1;
}

int Dispatcher::addReadFd(int fd)
{
  logTrace("Dispatcher::addReadFd");

  if (fd < 0 || FD_ISSET(fd, &pooledReadSet_))
  {
    logTest("Dispatcher::addReadFd", "Can't add descriptor %d "
                "to the read set", fd);

    logWarning("Dispatcher::addReadFd", ESET(EINVAL));

    return -1;
  }

  FD_SET(fd, &pooledReadSet_);

  increaseFds(fd, pooledReadSet_, lowerPooledReadFd_,
                  upperPooledReadFd_, firstPooledReadFd_,
                      pooledReadFds_);

  logTest("Dispatcher::addReadFd", "Added descriptor %d "
              "to the read set", fd);
  return 1;
}

int Dispatcher::removeReadFd(int fd)
{
  logTrace("Dispatcher::removeReadFd");

  if (!FD_ISSET(fd, &pooledReadSet_))
  {
    logTest("Dispatcher::removeReadFd", "Can't remove descriptor %d "
                "from the read set", fd);

    logWarning("Dispatcher::removeReadFd", ESET(EINVAL));

    return -1;
  }

  FD_CLR(fd, &pooledReadSet_);

  decreaseFds(fd, pooledReadSet_, lowerPooledReadFd_,
                  upperPooledReadFd_, firstPooledReadFd_,
                      pooledReadFds_);

  logTest("Dispatcher::removeReadFd", "Removed descriptor %d "
              "from the read set", fd);
  return 1;
}

int Dispatcher::addWriteFd(int fd)
{
  logTrace("Dispatcher::addWriteFd");

  if (fd < 0 || FD_ISSET(fd, &pooledWriteSet_))
  {
    logTest("Dispatcher::addWriteFd", "Can't add descriptor %d "
                "to the write set", fd);

    logWarning("Dispatcher::addWriteFd", ESET(EINVAL));

    return -1;
  }

  FD_SET(fd, &pooledWriteSet_);

  increaseFds(fd, pooledWriteSet_, lowerPooledWriteFd_,
                  upperPooledWriteFd_, firstPooledWriteFd_,
                      pooledWriteFds_);

  logTest("Dispatcher::addWriteFd", "Added descriptor %d "
              "to the write set", fd);
  return 1;
}

int Dispatcher::removeWriteFd(int fd)
{
  logTrace("Dispatcher::removeWriteFd");

  if (!FD_ISSET(fd, &pooledWriteSet_))
  {
    logTest("Dispatcher::removeWriteFd", "Can't remove descriptor %d "
                "from the write set", fd);

    logWarning("Dispatcher::removeWriteFd", ESET(EINVAL));

    return -1;
  }

  FD_CLR(fd, &pooledWriteSet_);

  decreaseFds(fd, pooledWriteSet_, lowerPooledWriteFd_,
                  upperPooledWriteFd_, firstPooledWriteFd_,
                      pooledWriteFds_);

  logTest("Dispatcher::removeWriteFd", "Removed descriptor %d "
              "from the write set", fd);
  return 1;
}

void Dispatcher::increaseFds(int fd, fd_set &set, int &lower, int &upper,
                                 int &first, int &total)
{
  logTest("Dispatcher::increaseFds", "Adding descriptor %d "
              "with lower %d, upper %d, first %d and total %d",
                  fd, lower, upper, first, total);

  total++;

  if (fd > upper)
  {
    upper = fd;

    while (!FD_ISSET(lower, &set) && lower < upper)
    {
      lower++;
    }
  }

  if (fd < lower)
  {
    lower = fd;

    while (!FD_ISSET(upper, &set) && upper > lower)
    {
      upper--;
    }
  }

  if (first > upper)
  {
    first = upper;
  }
  else if (first < lower)
  {
    first = lower;
  }

  logTest("Dispatcher::increaseFds", "Descriptors are now %d "
              "with lower %d, upper %d and first %d",
                  total, lower, upper, first);
}

void Dispatcher::decreaseFds(int fd, fd_set &set, int &lower, int &upper,
                                 int &first, int &total)
{
  logTest("Dispatcher::decreaseFds", "Removing descriptor %d "
              "with lower %d, upper %d, first %d and total %d",
                  fd, lower, upper, first, total);

  total--;

  while (!FD_ISSET(upper, &set) && upper > lower)
  {
    upper--;
  }

  while (!FD_ISSET(lower, &set) && lower < upper)
  {
    lower++;
  }

  if (first > upper)
  {
    first = upper;
  }
  else if (first < lower)
  {
    first = lower;
  }

  logTest("Dispatcher::decreaseFds", "Descriptors are now %d "
              "with lower %d, upper %d and first %d",
                  total, lower, upper, first);
}

