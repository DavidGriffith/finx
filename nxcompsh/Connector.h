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

#ifndef Connector_H
#define Connector_H

#include "Timestamp.h"
#include "Runnable.h"

enum T_connector_type
{
  connector_none = -1,
  connector_tcp,
  connector_unix
};

class Connector : protected Runnable
{
  protected:

  Connector(T_connector_type type);

  ~Connector();

  public:

  T_connector_type getType()
  {
    return type_;
  }

  int setHost(const char *host);
  int setPort(int port);

  int setDirectory(const char *directory);
  int setFile(const char *file);

  int setTimeout(const T_timestamp timeout);
 
  const char *getNode()
  {
    return node_;
  }

  int getFd()
  {
    return fd_;
  }

  int start();

  int end();

  protected:

  int startTcp();
  int startUnix();

  int makeNode();

  private:

  int fd_;

  int port_;
  char *host_;

  char *directory_;
  char *file_;

  char *node_;

  T_timestamp timeout_;

  T_connector_type type_;
};

class UnixConnector : public Connector
{
  public:

  UnixConnector() : Connector(connector_unix)
  {
  }
};

class TcpConnector : public Connector
{
  public:

  TcpConnector() : Connector(connector_tcp)
  {
  }
};

#endif /* Connector_H */
