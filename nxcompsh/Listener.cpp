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
#include <sys/stat.h>
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
#include "Listener.h"

Listener::Listener(T_listener_type type)
{
  logTrace("Listener::Listener");

  type_ = type;

  fd_   = -1;
  port_ = -1;

  accept_ = NULL;

  directory_ = NULL;
  file_      = NULL;

  node_ = NULL;

  //
  // By default will create the directory,
  // if any is given, and will remove the
  // socket on deletion.
  //

  create_ = 1;
  remove_ = 1;
  mode_   = -1;

  backlog_ = 8;
}

Listener::~Listener()
{
  logTrace("Listener::~Listener");

  end();

  delete [] directory_;
  delete [] accept_;
  delete [] file_;
  delete [] node_;
}

int Listener::setPort(int port)
{
  logTrace("Listener::setPort");

  if (type_ != listener_tcp)
  {
    logError("Listener::setPort", ESET(EPERM));

    return -1;
  }
  else if (port_ != -1)
  {
    logError("Listener::setPort", ESET(EPERM));

    return -1;
  }

  port_ = port;

  return 1;
}

int Listener::setBacklog(int value)
{
  logTrace("Listener::setBacklog");

  if (backlog_ <= 0)
  {
    logError("Listener::setBacklog", ESET(EINVAL));

    return -1;
  }

  backlog_ = value;

  return 1;
}

int Listener::setAccept(const char *host)
{
  logTrace("Listener::setAccept");

  if (type_ != listener_tcp || accept_ != NULL || fd_ != -1)
  {
    logError("Listener::setAccept", ESET(EPERM));

    return -1;
  }
  else if (host == NULL || *host == '\0')
  {
    logError("Listener::setAccept", ESET(EINVAL));

    return -1;
  }

  setValue(accept_, host);

  return 1;
}

int Listener::setDirectory(const char *directory)
{
  logTrace("Listener::setDirectory");

  if (type_ != listener_unix || directory_ != NULL || fd_ != -1)
  {
    logError("Listener::setDirectory", ESET(EPERM));

    return -1;
  }
  else if (directory == NULL || *directory == '\0')
  {
    logError("Listener::setDirectory", ESET(EINVAL));

    return -1;
  }

  setValue(directory_, directory);

  return 1;
}

int Listener::setFile(const char *file)
{
  logTrace("Listener::setFile");

  if (type_ != listener_unix || file_ != NULL || fd_ != -1)
  {
    logError("Listener::setFile", ESET(EPERM));

    return -1;
  }
  else if (file == NULL || *file == '\0')
  {
    logError("Listener::setFile", ESET(EINVAL));

    return -1;
  }

  setValue(file_, file);

  return 1;
}

int Listener::setCreate(int value)
{
  logTrace("Listener::setCreate");

  if (type_ != listener_unix || fd_ != -1)
  {
    logError("Listener::setCreate", ESET(EPERM));

    return -1;
  }
  else if (value != 0 && value != 1)
  {
    logError("Listener::setCreate", ESET(EINVAL));

    return -1;
  }

  create_ = value;

  return 1;
}

int Listener::setRemove(int value)
{
  logTrace("Listener::setRemove");

  if (type_ != listener_unix)
  {
    logError("Listener::setRemove", ESET(EPERM));

    return -1;
  }
  else if (value != 0 && value != 1)
  {
    logError("Listener::setRemove", ESET(EINVAL));

    return -1;
  }

  remove_ = value;

  return 1;
}

int Listener::setMode(int value)
{
  logTrace("Listener::setMode");

  if (type_ != listener_unix || fd_ != -1)
  {
    logError("Listener::setMode", ESET(EPERM));

    return -1;
  }

  mode_ = value;

  return 1;
}

int Listener::start()
{
  logTrace("Listener::start");

  if (fd_ != -1)
  {
    logError("Listener::start", ESET(EPERM));

    return -1;
  }

  switch (type_)
  {
    case listener_tcp:
    {
      if (port_ == -1)
      {
        logError("Listener::start", ESET(EPERM));

        return -1;
      }

      return startTcp();
    }
    case listener_unix:
    {
      if (file_ == NULL)
      {
        logError("Listener::start", ESET(EPERM));

        return -1;
      }

      return startUnix();
    }
    default:
    {
      logError("Listener::start", ESET(EPERM));

      return -1;
    }
  }
}

int Listener::accept()
{
  logTrace("Listener::accept");

  //
  // Get the IP address of host to be accepted.
  //

  int ip = 0;

  if (accept_ != NULL)
  {
    ip = GetHostAddress(accept_);

    if (ip <= 0)
    {
      logTest("Listener::accept", "Can't accept connections from "
                  "unknown host '%s'", accept_);

      logError("Listener::accept", ESET(ENETUNREACH));

      return -1;
    }
  }

  sockaddr_in address;

  size_t length = sizeof(sockaddr_in);

  int fd = ::accept(fd_, (sockaddr *) &address, (socklen_t *) &length);

  if (fd == -1)
  {
    logError("Listener::accept::accept", EGET());

    return -1;
  }

  char *host = inet_ntoa(address.sin_addr);

  unsigned int port = ntohs(address.sin_port);

  if (accept_ == NULL || (int) address.sin_addr.s_addr == ip)
  {
    logTest("Listener::accept", "Accepted connection from '%s' "
                "on port %d", host, port);

    return fd;
  }

  logTest("Listener::accept", "Refusing connection from '%s' "
              "on port %d", host, port);

  logError("Listener::accept", ESET(ECONNREFUSED));

  close(fd);

  return -1;
}

int Listener::makeNode()
{
  logTrace("Listener::makeNode");

  resetValue(node_);

  struct stat data;

  if (create_ == 1 && directory_ != NULL)
  {
    if ((stat(directory_, &data) == -1) && (EGET() == ENOENT))
    {
      logTest("Listener::makeNode", "Creating directory '%s'",
                  directory_);

      if (mkdir(directory_, 0755) == -1)
      {
        logError("Listener::makeNode::mkdir", EGET());

        goto ListenerMakeNodeError;
      }
    }
  }

  if (directory_ != NULL)
  {
    node_ = new char[strlen(directory_) + strlen(file_) + 2];

    if (node_ == NULL)
    {
      logError("Listener::makeNode::new", ESET(ENOMEM));

      goto ListenerMakeNodeError;
    }

    sprintf(node_, "%s/%s", directory_, file_);

    logTest("Listener::makeNode", "Assumed compound node name '%s'",
                node_);
  }
  else
  {
    node_ = new char[strlen(file_) + 1];

    if (node_ == NULL)
    {
      logError("Listener::makeNode::new", ESET(ENOMEM));

      goto ListenerMakeNodeError;
    }

    strcpy(node_, file_);

    logTest("Listener::makeNode", "Assumed simple node name '%s'",
                node_);
  }

  return 1;

ListenerMakeNodeError:

  resetValue(node_);

  return -1;
}

int Listener::startTcp()
{
  logTrace("Listener::startTcp");

  //
  // Start listening on the TCP socket.
  //

  if ((fd_ = socket(AF_INET, SOCK_STREAM, PF_UNSPEC)) == -1)
  {
    logError("Listener::startTcp::socket", EGET());

    goto ListenerStartTcpError;
  }
  else if (SetReuseAddress(fd_) == -1)
  {
    goto ListenerStartTcpError;
  }

  sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_port = htons(port_);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd_, (sockaddr *) &address, sizeof(address)) == -1)
  {
    logError("Listener::startTcp::bind", EGET());

    goto ListenerStartTcpError;
  }

  if (listen(fd_, backlog_) == -1)
  {
    logError("Listener::startTcp::listen", EGET());

    goto ListenerStartTcpError;
  }

  return 1;

ListenerStartTcpError:

  if (fd_ != -1)
  {
    close(fd_);
  }

  fd_ = -1;

  return -1;
}

int Listener::startUnix()
{
  logTrace("Listener::startUnix");

  //
  // Compose a valid node name.
  //

  if (makeNode() != 1)
  {
    return -1;
  }

  if (strlen(node_) >= 1024)
  {
    logError("Listener::startUnix", ESET(ENOMEM));

    goto ListenerStartUnixError;
  }

  //
  // Start listening on the Unix socket.
  //

  if ((fd_ = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC)) == -1)
  {
    logError("Listener::startUnix::socket", EGET());

    goto ListenerStartUnixError;
  }

  sockaddr_un address;

  address.sun_family = AF_UNIX;

  strcpy(address.sun_path, node_);

  if (bind(fd_, (sockaddr *) &address, sizeof(address)) == -1)
  {
    logError("Listener::startUnix::bind", EGET());

    goto ListenerStartUnixError;
  }

  //
  // Change access permissions of the socket.
  //

  if (mode_ != -1)
  {
    if (chmod(node_, mode_) == -1)
    {
      logError("Listener::startUnix::chmod", EGET());

      goto ListenerStartUnixError;
    }
  }

  if (listen(fd_, backlog_) == -1)
  {
    logError("Listener::startUnix::listen", EGET());

    goto ListenerStartUnixError;
  }

  return 1;

ListenerStartUnixError:

  if (remove_ == 1 && node_ != NULL)
  {
    unlink(node_);
  }

  resetValue(node_);

  if (fd_ != -1)
  {
    close(fd_);
  }

  fd_ = -1;

  return -1;
}

int Listener::end()
{
  logTrace("Listener::end");

  if (fd_ == -1)
  {
    return 0;
  }

  close(fd_);

  if (remove_ == 1 && node_ != NULL)
  {
    if (unlink(node_) == -1)
    {
      logError("Listener::end::unlink", EGET());

      return -1;
    }
  }

  return 1;
}

