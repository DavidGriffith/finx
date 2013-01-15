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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "Logger.h"
#include "Display.h"

int displayHandler();

//
// Main process.
//

int main(int argc, char *argv[])
{
/*
FIXME

  extern void RunTest();

  RunTest();

  exit(0);
*/

  logTest("RunTest", "Going to run the display handler");

  displayHandler();

  logTest("RunTest", "Going to exit the display handler");

  exit(0);
}

int displayHandler()
{
  logTest("displayHandler", "Going to create a new display");

  Display *display = new Display();

  //
  // Test manipulating the authority file
  // and the display name.
  //

  logTest("displayHandler", "Using authority file '%s'",
              display -> getAuthority());

  display -> setAuthority("/home/pinzari/.Xauthority");

  logTest("displayHandler", "Now using authority file '%s'",
              display -> getAuthority());

  display -> setAuthority("/home/pinzari/.Xauthority2");

  logTest("displayHandler", "Now using authority file '%s'",
              display -> getAuthority());

  display -> setHost("pippo");
  display -> setPort(65536);

  display -> setDisplay("localhost:0");

  //
  // Test manipulating the cookies.
  //

  char string[1024];

  *string = '\0';

  const char *cookie;

  cookie = display -> getCookie();

  if (cookie != NULL)
  {
    strcpy(string, cookie);
  }

  logTest("displayHandler", "Got cookie '%s' with display '%s'",
              string, display -> getDisplay());

  display -> setAuthority("");

  logTest("displayHandler", "Now using authority file '%s'",
              display -> getAuthority());

  cookie = display -> getCookie();

  if (cookie != NULL)
  {
    strcpy(string, cookie);
  }

  logTest("displayHandler", "Got cookie '%s' with display '%s'",
              string, display -> getDisplay());

  char *fake = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

  logTest("displayHandler", "Setting cookie to '%s' with display '%s'",
              fake, display -> getDisplay());

  display -> setCookie(fake);

  logTest("displayHandler", "Setting cookie to '%s' with display '%s'",
              string, display -> getDisplay());

  display -> setCookie(string);

  //
  // Test manipulating the listening sockets.
  //

  logTest("displayHandler", "Can bind to TCP returns %d",
              display -> canBindTcp());

  logTest("displayHandler", "Can bind to Unix returns %d",
              display -> canBindUnix());

  //
  // Test connecting to the existing X server.
  //

  display -> setTimeout(getTimestamp(5000));

  display -> setDisplay("192.168.2.200:0");

  logTest("displayHandler", "Can connect to display '%s' "
              "returns %d", display -> getDisplay(),
                  display -> canConnect());

  display -> setDisplay(":0");

  logTest("displayHandler", "Can connect to display '%s' "
              "returns %d", display -> getDisplay(),
                  display -> canConnect());

  display -> setAuthority("/home/pinzari/.Xauthority2");

  logTest("displayHandler", "Now using authority file '%s'",
              display -> getAuthority());

  cookie = display -> getCookie();

  if (cookie != NULL)
  {
    strcpy(string, cookie);
  }

  logTest("displayHandler", "Got cookie '%s' with display '%s'",
              string, display -> getDisplay());

  logTest("displayHandler", "Can connect to display '%s' "
              "returns %d", display -> getDisplay(),
                  display -> canConnect());

  //
  // Get a random cookie.
  //

  for (int i = 0; i < 10; i++)
  {
    cookie = display -> generateCookie();

    logTest("displayHandler", "Got random cookie '%s'", cookie);

    delete [] cookie;
  }

  //
  // Cleanup and exit.
  //

  logTest("displayHandler", "Going to delete the display");

  delete display;

  logTest("displayHandler", "Deleted the display");

  return 1;
}
