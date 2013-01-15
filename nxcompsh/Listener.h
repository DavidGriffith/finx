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

#ifndef Listener_H
#define Listener_H

#include "Runnable.h"

enum T_listener_type
{
  listener_none = -1,
  listener_tcp,
  listener_unix
};

class Listener : protected Runnable
{
  protected:

  Listener(T_listener_type type);

  ~Listener();

  public:

  T_listener_type getType()
  {
    return type_;
  }

  int setPort(int port);

  int setAccept(const char *host);

  int setDirectory(const char *directory);
  int setFile(const char *file);

  const char *getNode()
  {
    return node_;
  }

  int getFd()
  {
    return fd_;
  }

  int setCreate(int value);
  int setRemove(int value);
  int setMode(int value);

  int setBacklog(int value);

  int start();

  int accept();

  int end();

  protected:

  int startTcp();
  int startUnix();

  int makeNode();

  private:

  int fd_;
  int backlog_;

  int port_;
  char *accept_;

  char *directory_;
  char *file_;

  char *node_;

  int mode_;
  int create_;
  int remove_;

  T_listener_type type_;
};

class UnixListener : public Listener
{
  public:

  UnixListener() : Listener(listener_unix)
  {
  }
};

class TcpListener : public Listener
{
  public:

  TcpListener() : Listener(listener_tcp)
  {
  }
};

#endif /* Listener_H */
