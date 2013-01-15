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

#ifndef Display_H
#define Display_H

#include "Timestamp.h"

class Display
{
  public:

  Display();

  ~Display();

  public:

  int setDisplay(const char *display);
  int setHost(const char *host);
  int setPort(int port);

  const char *getDisplay()
  {
    return display_;
  }

  const char *getHost()
  {
    return host_;
  }

  int getPort()
  {
    return port_;
  }

  int canBind()
  {
    return (canBindTcp() == 1 &&
                canBindUnix() == 1);
  }

  int canBindTcp();
  int canBindUnix();

  int canConnect();

  int setAuthority(const char *file);
  const char *getAuthority();

  int setCookie(const char *cookie);
  const char *getCookie();

  int setTimeout(const T_timestamp timeout);

  char *generateCookie();

  private:

  int connectTcp();
  int connectUnix();

  int checkCookie(int fd);

  int saveHost(const char *host, int length);
  int savePort(int port);
  int saveDisplay();

  const char *authCommand();

  int  port_;

  char *host_;
  char *display_;
  char *authority_;
  char *cookie_;

  T_timestamp timeout_;
};

#endif /* Display_H */
