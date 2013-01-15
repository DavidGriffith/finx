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

#ifndef Dispatcher_H
#define Dispatcher_H

#include <sys/select.h>

#include "Timestamp.h"
#include "Runnable.h"
#include "Event.h"

class Dispatcher
{
  public:

  Dispatcher();

  ~Dispatcher();

  int start();

  int addReadFd(int fd);

  int removeReadFd(int fd);

  int addWriteFd(int fd);

  int removeWriteFd(int fd);

  int waitEvent(T_timestamp &timeout);

  int nextReadEvent();

  int end();

  protected:

  void increaseFds(int fd, fd_set &set, int &lower, int &upper,
                       int &first, int &total);

  void decreaseFds(int fd, fd_set &set, int &lower, int &upper,
                       int &first, int &total);

  private:

  fd_set readSet_;
  fd_set writeSet_;

  int resultFds_;
  int limitFd_;

  T_timestamp selectTs_;

  fd_set pooledReadSet_;
  fd_set pooledWriteSet_;

  int pooledReadFds_;
  int pooledWriteFds_;

  int lowerPooledReadFd_;
  int upperPooledReadFd_;
  int firstPooledReadFd_;

  int lowerPooledWriteFd_;
  int upperPooledWriteFd_;
  int firstPooledWriteFd_;
};

#endif /* Dispatcher_H */
