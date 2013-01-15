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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "System.h"
#include "Logger.h"
#include "Display.h"
#include "Process.h"
#include "Listener.h"
#include "Connector.h"

#define NX_UNIX_SOCKET_DIR  "/tmp/.X11-unix"

#define NX_INVALID_COOKIE_DATA  "Invalid MIT-MAGIC-COOKIE-1 key"
#define NX_INVALID_COOKIE_SIZE  ((int) sizeof(NX_INVALID_COOKIE_DATA) - 1)

#define NX_NO_AUTH_PROTO_DATA   "No protocol specified"
#define NX_NO_AUTH_PROTO_SIZE   ((int) sizeof(NX_NO_AUTH_PROTO_DATA) - 1)

Display::Display()
{
  logTrace("Display::Display");

  port_ = -1;

  host_      = NULL;
  display_   = NULL;
  authority_ = NULL;
  cookie_    = NULL;

  timeout_ = nullTimestamp();
}

Display::~Display()
{
  logTrace("Display::~Display");

  delete [] host_;
  delete [] display_;
  delete [] authority_;
  delete [] cookie_;
}

int Display::saveHost(const char *host, int length)
{
  logTrace("Display::saveHost");

  if (host_ != NULL)
  {
    logTest("Display::saveHost", "Replacing previous "
                "host specification '%s'", host_);

    delete [] host_;
  }

  if ((host_ = new char[length + 1]) == NULL)
  {
    logError("Display::saveHost::new", ESET(ENOMEM));

    return -1;
  }

  strncpy(host_, host, length);

  *(host_ + length) = '\0';

  return 1;
}

int Display::savePort(int port)
{
  logTrace("Display::savePort");

  if (port < 0 || port + 6000 > 65535)
  {
    logError("Display::savePort", ESET(EINVAL));

    return -1;
  }

  port_ = port;

  return 1;
}

int Display::saveDisplay()
{
  logTrace("Display::saveDisplay");

  if (host_ == NULL || port_ < 0)
  {
    logError("Display::saveDisplay", ESET(EPERM));

    return -1;
  }

  if (display_ != NULL)
  {
    logTest("Display::saveDisplay", "Replacing previous "
                "display '%s'", display_);

    delete [] display_;
  }

  char line[1024];

  snprintf(line, 1023, "%s:%d", host_, port_);

  *(line + sizeof(line) - 1) = '\0';

  if ((display_ = new char[strlen(line) + 1]) == NULL)
  {
    logError("Display::saveDisplay::new", ESET(ENOMEM));

    return -1;
  }

  strcpy(display_, line);

  logTest("Display::saveDisplay", "Host is '%s' "
             "port is %d", host_, port_);

  logTest("Display::saveDisplay", "Set display to "
              "'%s'", display_);

  logTest("Display::saveDisplay", "Removing any "
              "previous cookie");

  delete [] cookie_;

  cookie_ = NULL;
    
  return 1;
}

int Display::setHost(const char *host)
{
  logTrace("Display::setHost");

  if (saveHost(host, strlen(host)) < 0)
  {
    return -1;
  }

  if (port_ >= 0)
  {
    return saveDisplay();
  }

  return 1;
}

int Display::setPort(int port)
{
  logTrace("Display::setPort");

  if (savePort(port) < 0)
  {
    return -1;
  }

  if (host_ != NULL)
  {
    return saveDisplay();
  }

  return 1;
}

int Display::setDisplay(const char *display)
{
  logTrace("Display::setDisplay");

  if (display_ != NULL &&
        strcmp(display_, display) == 0)
  {
    return 1;
  }

  //
  // Look for a host name followed
  // by a colon followed by port.
  //

  const char *separator = strrchr(display, ':');

  if (separator == NULL)
  {
    logTest("Display::setDisplay", "Invalid display "
                "'%s'", display);

    logError("Display::setDisplay", ESET(EINVAL));

    return -1;
  }

  const char *check = separator + 1;

  while (*check != '\0' && isdigit(*check) != 0)
  {
    check++;
  }

  if (*check != '\0' || savePort(atoi(separator + 1)) < 0)
  {
    if (*check != '\0')
    {
      logTest("Display::setDisplay", "Invalid port in "
                  "display '%s'", display);

      logError("Display::setDisplay", ESET(EINVAL));
    }

    return -1;
  }

  if (saveHost(display, strlen(display) - strlen(separator)) < 0)
  {
    return -1;
  }

  return saveDisplay();
}

int Display::setAuthority(const char *file)
{
  logTrace("Display::setAuthority");

  if (authority_ != NULL &&
          strcmp(authority_, file) == 0)
  {
    return 1;
  }

  if (authority_ != NULL)
  {
    logTest("Display::setAuthority", "Replacing previous "
                "authority file '%s'", authority_);

    delete [] authority_;
  }

  if ((authority_ = new char[strlen(file) + 1]) == NULL)
  {
    logError("Display::setAuthority::new", ESET(ENOMEM));

    return -1;
  }

  strcpy(authority_, file);

  logTest("Display::setAuthority", "Removing any "
              "previous cookie");

  delete [] cookie_;

  cookie_ = NULL;
    
  return 1;
}

const char *Display::getAuthority()
{
  logTrace("Display::getAuthority");

  if (authority_ != NULL)
  {
    return authority_;
  }

  char *file;

  file = getenv("XAUTHORITY");

  if (file != NULL)
  {
    if ((authority_ = new char[strlen(file) + 1]) == NULL)
    {
      logError("Display::getAuthority::new", ESET(ENOMEM));

      return NULL;
    }

    strcpy(authority_, file);
  }
  else
  {
    char *home = getenv("HOME");

    if (home == NULL)
    {
      logTest("Display::getAuthority", "No environment "
                  "variable for HOME");

      logError("Display::getAuthority", ESET(ENOENT));

      return NULL;
    }

    if ((authority_ = new char[strlen(home) + 12 + 1]) == NULL)
    {
      logError("Display::getAuthority::new", ESET(ENOMEM));

      return NULL;
    }

    strcpy(authority_, home);
    strcat(authority_, "/.Xauthority");
  }

  logTest("Display::getAuthority", "Using authority "
              "file '%s'", authority_);

  return authority_;
}

int Display::setTimeout(const T_timestamp timeout)
{
  logTrace("Display::setTimeout");

  timeout_ = timeout;

  return 1;
}

const char *Display::authCommand()
{
  logTrace("Display::authCommand");

  char *path;

  if ((path = new char[1024]) == NULL)
  {
    logError("Display::authCommand::new", ESET(ENOMEM));

    return NULL;
  }

  //
  // Use the nxauth command on Windows and the Mac, xauth
  // on all the other platforms. This should be improved
  // by checking if any of the two are in our path and by
  // retrying using other well-known paths that are like-
  // ly to contain the program.
  //

  const char *command;

  #if defined(__CYGWIN32__) || defined(__APPLE__)

  command = "nxauth";

  #else

  command = "xauth";

  #endif

  //
  // If the user selected a NX system directory,
  // then search the command there.
  //

  const char *system = getSystemDir();

  if (system != NULL)
  {
    strncpy(path, system,  1023);
    strncat(path, "/bin/", 1023 - strlen(path));
    strncat(path, command, 1023 - strlen(path));
  }
  else
  {
    strncpy(path, command, 1023);
  }

  logTest("Display::authCommand", "Using X auth "
              "command '%s'", path);

  return path;
}

const char *Display::getCookie()
{
  logTrace("Display::getCookie");

  //
  // Return the previously acquired cookie
  // in the case of multiple invocations.
  //

  if (cookie_ != NULL)
  {
    return cookie_;
  }

  if (port_ < 0 || host_ == NULL)
  {
    logTest("Display::getCookie", "No display "
                "set for the command");

    logError("Display::getCookie", ESET(EPERM));

    return NULL;
  }

  Process *process = NULL;

  //
  // Get the appropriate authority file. If
  // the file is set to an empty string we
  // will let the command use its default.
  //

  const char *authority = getAuthority();

  //
  // Get the appropriate command to be used
  // to get the cookie.
  //

  const char *command = NULL;

  if ((command = authCommand()) == NULL)
  {
    goto DisplayGetCookieError;
  }

  //
  // The SSH code forces using the unix:n port when passing localhost:n.
  // This is probably because localhost:n can fail to return a valid
  // entry on machines where the hostname for localhost doesn't match
  // exactly the 'localhost' string. For example, on a freshly installed
  // Fedora Core 3 I get a 'localhost.localdomain/unix:0' entry. Query-
  // ing 'xauth list localhost:0' results in an empty result, while the
  // query 'xauth list unix:0' works as expected. Note anyway that if
  // the cookie for the TCP connection on 'localhost' is set to a dif-
  // ferent cookie than the one for the Unix connections, both SSH and
  // NX will match the wrong cookie and session will fail.
  //

  char line[256];

  if (strncmp(host_, "localhost", 9) == 0)
  {
    snprintf(line, 256, "unix:%d", port_);
  }
  else
  {
    snprintf(line, 256, "%.200s:%d", host_, port_);
  }

  *(line + sizeof(line) - 1) = '\0';

  if ((process = new Process()) == NULL)
  {
    logError("Display::getCookie::new", ESET(ENOMEM));

    goto DisplayGetCookieError;
  }

  process -> setCommand(command);

  if (*authority != '\0')
  {
    process -> addParameter("-f");
    process -> addParameter(authority);
  }

  process -> addParameter("list");
  process -> addParameter(line);

  if (process -> start() < 0)
  {
    logError("Display::getCookie", EGET());

    goto DisplayGetCookieError;
  }

  logTest("Display::getCookie", "Command started "
              "with pid %d", process -> getPid());

  FILE *stream;

  if ((stream =  process -> getStdoutStream()) == NULL)
  {
    logError("Display::getCookie", EGET());

    goto DisplayGetCookieError;
  }

  if (fgets(line, 1024, stream) == NULL)
  {
    logWarning("Display::getCookie", ESET(ENOENT));

    logTest("Display::getCookie", "Failed to read "
                "data from the X auth command");

    goto DisplayGetCookieError;
  }

  logTest("Display::getCookie", "Checking cookie "
              "in string '%s'", line);

  char cookie[512];

  if (sscanf(line, "%*s %*s %511s", cookie) != 1)
  {
    logWarning("Display::getCookie", ESET(ENOENT));

    logTest("Display::getCookie", "Failed to identify "
                "the cookie in string '%s'", line);

    goto DisplayGetCookieError;
  }

  if (cookie_ != NULL)
  {
    logTest("Display::getCookie", "Replacing previous "
                "cookie '%s'", cookie_);

    delete cookie_;
  }

  if ((cookie_ = new char[strlen(cookie) + 1]) == NULL)
  {
    logError("Display::getCookie::new", ESET(ENOMEM));

    goto DisplayGetCookieError;
  }

  strcpy(cookie_, cookie);

  logTest("Display::getCookie", "Gotten cookie '%s' "
              "from file '%s'", cookie_, authority_);

  delete process;
  delete [] command;

  return cookie_;

DisplayGetCookieError:

  if (process != NULL)
  {
    process -> end();

    logTest("Display::getCookie", "Waiting process "
                "with pid %d", process -> getPid());

    process -> wait();
  }

  delete process;
  delete [] command;

  delete cookie_;

  cookie_ = NULL;

  return NULL;
}

int Display::setCookie(const char *cookie)
{
  logTrace("Display::setCookie");

  //
  // Don't write the cookie more than once.
  //

  if (cookie_ != NULL &&
          strcmp(cookie_, cookie) == 0)
  {
    return 1;
  }

  if (strlen(cookie) != 32)
  {
    logTest("Display::setCookie", "Cookie must be "
                "a string 32 bytes long");

    logError("Display::setCookie", ESET(EINVAL));

    return -1;
  }
  else if (port_ < 0 || host_ == NULL)
  {
    logTest("Display::setCookie", "No display "
                "set for the command");

    logError("Display::setCookie", ESET(EPERM));

    return -1;
  }

  //
  // The code is the same used for getting the
  // cookie, so refer to the other procedure
  // for comments.

  Process *process = NULL;

  const char *authority = getAuthority();

  const char *command = NULL;

  if ((command = authCommand()) == NULL)
  {
    goto DisplaySetCookieError;
  }

  char line[1024];

  if (strncmp(host_, "localhost", 9) == 0)
  {
    snprintf(line, 1024, "unix:%d", port_);
  }
  else
  {
    snprintf(line, 1024, "%.200s:%d", host_, port_);
  }

  *(line + sizeof(line) - 1) = '\0';

  if ((process = new Process()) == NULL)
  {
    logError("Display::setCookie::new", ESET(ENOMEM));

    goto DisplaySetCookieError;
  }

  process -> setCommand(command);

  if (*authority != '\0')
  {
    process -> addParameter("-f");
    process -> addParameter(authority);
  }

  process -> addParameter("add");
  process -> addParameter(line);
  process -> addParameter("MIT-MAGIC-COOKIE-1");
  process -> addParameter(cookie);

  if (process -> start() < 0)
  {
    logError("Display::setCookie", EGET());

    goto DisplaySetCookieError;
  }

  logTest("Display::setCookie", "Waiting process "
              "with pid %d", process -> getPid());

  process -> wait();

  delete [] cookie_;

  cookie_ = NULL;

  if (process -> isSuccess() == 1)
  {
    if ((cookie_ = new char[strlen(cookie) + 1]) != NULL)
    {
      strcpy(cookie_, cookie);

      logTest("Display::setCookie", "Set cookie to '%s' "
                  "in file '%s'", cookie_, authority_);
    }
  }

  delete process;
  delete [] command;

  if (cookie_ == NULL)
  {
    logTest("Display::setCookie", "Command failed");

    logError("Display::setCookie", ESET(EPERM));

    return -1;
  }

  return 1;

DisplaySetCookieError:

  delete process;
  delete [] command;

  return -1;
}

int Display::canBindTcp()
{
  logTrace("Display::canBindTcp");

  if (port_ < 0 || host_ == NULL)
  {
    logTest("Display::canBindTcp", "No display "
                "set for the command");

    logError("Display::canBindTcp", ESET(EPERM));

    return 0;
  }

  TcpListener *listener;

  if ((listener = new TcpListener()) == NULL)
  {
    logError("Display::canBindTcp", ESET(ENOMEM));

    return 0;
  }

  int result = (listener -> setPort(port_ + 6000) == 1 &&
                    listener -> start() == 1);

  delete listener;

  return result;
}

int Display::canBindUnix()
{
  logTrace("Display::canBindUnix");

  if (port_ < 0 || host_ == NULL)
  {
    logTest("Display::canBindUnix", "No display "
                "set for the command");

    logError("Display::canBindUnix", ESET(EPERM));

    return 0;
  }
  else if (strcmp(host_, "localhost") != 0 &&
               strcmp(host_, "unix") != 0 &&
                   strcmp(host_, "") != 0)
  {
    logTest("Display::canBindUnix", "Not a local "
                "display set for the command");

    logError("Display::canBindUnix", ESET(EINVAL));

    return 0;
  }

  UnixListener *listener;

  if ((listener = new UnixListener()) == NULL)
  {
    logError("Display::canBindUnix", ESET(ENOMEM));

    return 0;
  }

  //
  // Automatically create the X socket directory
  // if it does not exist. It may be preferrable
  // to let the user specify if this has to be
  // the function's behaviour.
  //

  struct stat data;

  if ((stat(NX_UNIX_SOCKET_DIR, &data) == -1) &&
          (EGET() == ENOENT))
  {
    if (mkdir(NX_UNIX_SOCKET_DIR, (0777 | S_ISVTX)) != 0 ||
            chmod(NX_UNIX_SOCKET_DIR, (0777 | S_ISVTX)) != 0)
    {
      logError("Display::canBindUnix", EGET());

      logTest("Display::canBindUnix", "Can't create "
                  "the '%s' directory", NX_UNIX_SOCKET_DIR);

      return 0;
    }
  }

  char line[1024];

  snprintf(line, 1023, "%s/X%d", NX_UNIX_SOCKET_DIR, port_);

  *(line + sizeof(line) - 1) = '\0';

  int result = (listener -> setFile(line) == 1 &&
                    listener -> start() == 1);

  delete listener;

  return result;
}

int Display::canConnect()
{
  logTrace("Display::canConnect");

  if (port_ < 0 || host_ == NULL)
  {
    logTest("Display::canConnect", "No display "
                "set for the command");

    logError("Display::canConnect", ESET(EPERM));

    return 0;
  }

  if (strcmp(host_, "localhost") == 0 ||
          strcmp(host_, "unix") == 0 ||
              strcmp(host_, "") == 0)
  {
    return (connectUnix() == 1);
  }
  else
  {
    return (connectTcp() == 1);
  }
}

int Display::connectTcp()
{
  logTrace("Display::connectTcp");

  TcpConnector *connector;

  if ((connector = new TcpConnector()) == NULL)
  {
    logError("Display::connectTcp", ESET(ENOMEM));

    return -1;
  }

  int result = 1;

  if (connector -> setHost(host_) != 1 ||
          connector -> setPort(port_ + 6000) != 1 ||
              (isTimestamp(timeout_) == 1 &&
                  connector -> setTimeout(timeout_) != 1) ||
                      connector -> start() != 1)
  {
    logTest("Display::connectTcp", "Failed to "
                "connect to display '%s'", display_);

    result = 0;
  }
  else
  {
    int fd = connector -> getFd();

    result = checkCookie(fd);

    close(fd);
  }

  delete connector;

  return result;
}

int Display::connectUnix()
{
  logTrace("Display::connectUnix");

  UnixConnector *connector;

  if ((connector = new UnixConnector()) == NULL)
  {
    logError("Display::connectUnix", ESET(ENOMEM));

    return -1;
  }

  int result = 1;

  char line[1024];

  snprintf(line, 1023, "%s/X%d", NX_UNIX_SOCKET_DIR, port_);

  *(line + sizeof(line) - 1) = '\0';

  if (connector -> setFile(line) != 1 ||
          (isTimestamp(timeout_) == 1 &&
              connector -> setTimeout(timeout_) != 1) ||
                      connector -> start() != 1)
  {
    logTest("Display::connectUnix", "Failed to "
                "connect to display '%s'", display_);

    result = 0;
  }
  else
  {
    int fd = connector -> getFd();

    result = checkCookie(fd);

    close(fd);
  }

  delete connector;

  return result;
}

int Display::checkCookie(int fd)
{
  logTrace("Display::checkCookie");

  //
  // This code mimic the connection pocedure of a X client.
  // We, actually, don't care the byte order or any additi-
  // onal data reported by the X server, but only want to
  // verify if the X server will accept our authorization
  // credentials.
  //

  char dataWithCookie[48] =
  {
    0x6c, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x12, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x4d, 0x49, 0x54, 0x2d,
    0x4d, 0x41, 0x47, 0x49, 0x43, 0x2d, 0x43, 0x4f,
    0x4f, 0x4b, 0x49, 0x45, 0x2d, 0x31, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  char dataWithoutCookie[12] =
  {
    0x6c, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
  };

  char *data;
  int  size;

  if (cookie_ == NULL)
  {
    logTest("Display::checkCookie", "Trying without "
                "a cookie on descriptor %d", fd);

    data = dataWithoutCookie;
    size = sizeof(dataWithoutCookie);
  }
  else
  {
    logTest("Display::checkCookie", "Trying with "
                "cookie '%s' on descriptor %d",
                    cookie_, fd);

    data = dataWithCookie;
    size = sizeof(dataWithCookie);

    //
    // Put our cookie in the buffer.
    //

    int value;

    for (int i = 0; i < 16; i++)
    {
      if (sscanf(cookie_ + 2 * i, "%2x", &value) != 1)
      {
        logTest("Display::checkCookie", "Invalid data "
                    "in cookie '%s'", cookie_);

        return 0;
      }
      else if (32 + i <= size)
      {
        data[32 + i] = value;
      }
    }
  }

  int result;

  if ((result = write(fd, data, size)) != size)
  {
    logTest("Display::checkCookie", "Failed to write "
                "X connection data");

    return 0;
  }

  //
  // Check the X server's reply.
  //

  char reply[48];

  result = read(fd, reply, 48);

  if (result < 32)
  {
    logTest("Display::checkCookie", "Failed to read "
                "X reply data");

    return 0;
  }
  else if (*reply != 1)
  {
    #ifdef TEST

    if (result >= NX_INVALID_COOKIE_SIZE + 8 &&
            memcmp(reply + 8, NX_INVALID_COOKIE_DATA,
                       NX_INVALID_COOKIE_SIZE) == 0)
    {
      logTest("Display::checkCookie", "X server reported "
                  "error '%s'", NX_INVALID_COOKIE_DATA);
    }
    else if (result >= NX_NO_AUTH_PROTO_SIZE + 8 &&
                 memcmp(reply + 8, NX_NO_AUTH_PROTO_DATA,
                            NX_NO_AUTH_PROTO_SIZE) == 0)
    {
      logTest("Display::checkCookie", "X server reported "
                  "error '%s'", NX_NO_AUTH_PROTO_DATA);
    }
    else
    {
      logTest("Display::checkCookie", "X server reported "
                  "an unknown error");

      logDump("Display::checkCookie", reply, result);
    }

    #endif

    return 0;
  }

  logTest("Display::checkCookie", "Successfully "
              "negotiated the X connection");

  return 1;
}

char *Display::generateCookie()
{
  logTrace("Display::generateCookie");

  char *cookie;

  if ((cookie = new char[33]) == NULL)
  {
    logError("Display::generateCookie::new", ESET(ENOMEM));

    return NULL;
  }

  //
  // Should maybe use a much stronger random number generator.
  // For now we try to use 32 out of the 32 or more available
  // bits in a unsigned int, by concatenating the lower order
  // bits of the microseconds returned by gettimeofday(), to
  // create an appropriate seed. A bit of further randomness
  // is achieved by yielding control to the OS before getting
  // the second timestamp.
  //

  T_timestamp timer1 = getTimestamp();

  usleep(20);

  T_timestamp timer2 = getTimestamp();

  unsigned int seed = (timer1.tv_usec << 16) |
                          (timer2.tv_usec & 0xffff);

  srand(seed);

  unsigned int number = rand();

  for (int i = 0; i < 16; i++)
  {
    if (i % 4 == 0)
    {
      number = rand();
    }

    snprintf(cookie + 2 * i, 3, "%02x", number & 0xff);

    number >>= 8;
  }

  logTest("Display::generateCookie", "Generated cookie "
              "string '%s'", cookie);

  return cookie;
}
