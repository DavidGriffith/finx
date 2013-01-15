/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXCOMPSH, NX protocol compression NX extensions to this software       */
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
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "Logger.h"
#include "Socket.h"
#include "Connector.h"

Connector::Connector(T_connector_type type)
{
  logTrace("Connector::Connector");

  type_ = type;

  fd_   = -1;
  port_ = -1;

  host_ = NULL;

  directory_ = NULL;
  file_      = NULL;

  node_ = NULL;

  timeout_ = nullTimestamp();
}

Connector::~Connector()
{
  logTrace("Connector::~Connector");

  end();

  delete [] host_;
  delete [] directory_;
  delete [] file_;
  delete [] node_;
}

int Connector::setHost(const char *host)
{
  logTrace("Connector::setHost");

  if (type_ != connector_tcp || host_ != NULL || fd_ != -1)
  {
    logError("Connector::setHost", ESET(EPERM));

    return -1;
  }
  else if (host == NULL || *host == '\0')
  {
    logError("Connector::setAccept", ESET(EINVAL));

    return -1;
  }

  setValue(host_, host);

  return 1;
}

int Connector::setPort(int port)
{
  logTrace("Connector::setPort");

  if (type_ != connector_tcp)
  {
    logError("Connector::setPort", ESET(EPERM));

    return -1;
  }
  else if (port_ != -1)
  {
    logError("Connector::setPort", ESET(EPERM));

    return -1;
  }

  port_ = port;

  return 1;
}

int Connector::setDirectory(const char *directory)
{
  logTrace("Connector::setDirectory");

  if (type_ != connector_unix || directory_ != NULL || fd_ != -1)
  {
    logError("Connector::setDirectory", ESET(EPERM));

    return -1;
  }
  else if (directory == NULL || *directory == '\0')
  {
    logError("Connector::setDirectory", ESET(EINVAL));

    return -1;
  }

  setValue(directory_, directory);

  return 1;
}

int Connector::setFile(const char *file)
{
  logTrace("Connector::setFile");

  if (type_ != connector_unix || file_ != NULL || fd_ != -1)
  {
    logError("Connector::setFile", ESET(EPERM));

    return -1;
  }
  else if (file == NULL || *file == '\0')
  {
    logError("Connector::setFile", ESET(EINVAL));

    return -1;
  }

  setValue(file_, file);

  return 1;
}

int Connector::setTimeout(const T_timestamp timeout)
{
  logTrace("Connector::setTimeout");

  if (fd_ != -1)
  {
    logError("Connector::setFile", ESET(EPERM));

    return -1;
  }

  timeout_ = timeout;

  return 1;
}

int Connector::makeNode()
{
  logTrace("Connector::makeNode");

  resetValue(node_);

  if (directory_ != NULL)
  {
    node_ = new char[strlen(directory_) + strlen(file_) + 2];

    if (node_ == NULL)
    {
      logError("Connector::makeNode::new", ESET(ENOMEM));

      goto ConnectorMakeNodeError;
    }

    sprintf(node_, "%s/%s", directory_, file_);

    logTest("Connector::makeNode", "Assumed compound node name '%s'",
                node_);
  }
  else
  {
    node_ = new char[strlen(file_) + 1];

    if (node_ == NULL)
    {
      logError("Connector::makeNode::new", ESET(ENOMEM));

      goto ConnectorMakeNodeError;
    }

    strcpy(node_, file_);

    logTest("Connector::makeNode", "Assumed simple node name '%s'",
                node_);
  }

  return 1;

ConnectorMakeNodeError:

  resetValue(node_);

  return -1;
}

int Connector::start()
{
  logTrace("Connector::start");

  if (fd_ != -1)
  {
    logError("Connector::start", ESET(EPERM));

    return -1;
  }

  switch (type_)
  {
    case connector_tcp:
    {
      if (host_ == NULL || port_ == -1)
      {
        logError("Connector::start", ESET(EPERM));

        return -1;
      }

      return startTcp();
    }
    case connector_unix:
    {
      if (file_ == NULL)
      {
        logError("Connector::start", ESET(EPERM));

        return -1;
      }

      return startUnix();
    }
    default:
    {
      logError("Connector::start", ESET(EPERM));

      return -1;
    }
  }
}

int Connector::startTcp()
{
  logTrace("Connector::startTcp");

  int ip;

  if ((ip = GetHostAddress(host_)) == 0)
  {
    logTest("Connector::startTcp", "Can't connect to "
                "unknown host '%s'", host_);

    logError("Listener::startTcp", EGET());

    return -1;
  }

  sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_port = htons(port_);
  address.sin_addr.s_addr = ip;

  if ((fd_ = socket(AF_INET, SOCK_STREAM, PF_UNSPEC)) == -1)
  {
    logError("Connector::startTcp::socket", EGET());

    goto ConnectorStartTcpError;
  }

  logTest("Connector::startTcp", "Connecting to '%s' "
              "on port %d", host_, port_);

  if (isTimestamp(timeout_) == 1)
  {
    setTimer(timeout_);
  }

  if (connect(fd_, (sockaddr *) &address, sizeof(address)) == -1)
  {
    if (EGET() == EINTR && isTimestamp(timeout_))
    {
      logTest("Connector::startTcp::connect", "Timeout "
                  "raised connecting to '%s' on "
                      "port %d", host_, port_);
    }
    else
    {
      logError("Connector::startTcp::connect", EGET());
    }

    goto ConnectorStartTcpError;
  }

  if (isTimestamp(timeout_) == 1)
  {
    resetTimer();
  }

  logTest("Connector::startTcp", "Connected to '%s' "
              "on port %d", host_, port_);

  return 1;

ConnectorStartTcpError:

  if (fd_ != -1)
  {
    close(fd_);
  }

  fd_ = -1;

  return -1;
}

int Connector::startUnix()
{
  logTrace("Connector::startUnix");

  //
  // Compose a valid node name.
  //

  if (makeNode() != 1)
  {
    return -1;
  }

  if (strlen(node_) >= 1024)
  {
    logError("Connector::startUnix", ESET(ENOMEM));

    goto ConnectorStartUnixError;
  }

  if ((fd_ = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC)) == -1)
  {
    logError("Connector::startUnix::socket", EGET());

    goto ConnectorStartUnixError;
  }

  sockaddr_un address;

  address.sun_family = AF_UNIX;

  strcpy(address.sun_path, node_);

  logTest("Connector::startUnix", "Connecting "
              "to '%s'", node_);

  if (isTimestamp(timeout_) == 1)
  {
    setTimer(timeout_);
  }

  if (connect(fd_, (sockaddr *) &address, sizeof(address)) == -1)
  {
    if (EGET() == EINTR && isTimestamp(timeout_))
    {
      logTest("Connector::startUnix::connect", "Timeout "
                  "raised connecting to '%s'", node_);
    }
    else
    {
      logError("Connector::startUnix::connect", EGET());
    }

    goto ConnectorStartUnixError;
  }

  if (isTimestamp(timeout_) == 1)
  {
    resetTimer();
  }

  logTest("Connector::startUnix", "Connected "
              "to '%s' ", node_);

  return 1;

ConnectorStartUnixError:

  resetValue(node_);

  if (fd_ != -1)
  {
    close(fd_);
  }

  fd_ = -1;

  return -1;
}

int Connector::end()
{
  logTrace("Connector::end");

  if (fd_ == -1)
  {
    return 0;
  }

  fd_ = -1;

  return 1;
}

