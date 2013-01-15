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
#include <fcntl.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "Misc.h"
#include "Logger.h"
#include "Timestamp.h"
#include "Process.h"
#include "Listener.h"
#include "Dispatcher.h"
#include "Request.h"

#define DEFAULT_PROCESS_ENTRIES  256
#define DEFAULT_BUFFER_LENGTH    16384

/*
FIXME: This is just a test.
*/

//
// Global data.
//

Process *processes[DEFAULT_PROCESS_ENTRIES];
Dispatcher *dispatcher;
TcpListener *listener;
int requests;

//
// Dispatch handlers.
//

int connectionHandler();
int processHandler(int fd);
int processCheck();
int processWait();

//
// Request handler.
//

int requestHandler(void *);

//
// Main process.
//

void RunTest()
{
/*
FIXME

  extern void InstallSignals();

  logTest("RunTest", "Going to install the signal handlers");

  InstallSignals();
*/
  logTest("RunTest", "Going to run the connection handler");

  connectionHandler();

  logTest("RunTest", "Going to exit with %d requests handled",
              requests);
/*
FIXME

  HandleCleanup();
*/
}

int connectionHandler()
{
  requests = 0;

  logTest("connectionHandler", "Going to create a new dispatcher");

  dispatcher = new Dispatcher();

  dispatcher -> start();

  logTest("connectionHandler", "Going to create a new listener");

  listener = new TcpListener();

  listener -> setPort(6001);
  listener -> setBacklog(32);

  //
  // Not needed for the early tests.
  //
  // listener -> setAccept("127.0.0.1");
  //

  listener -> start();

  logTest("connectionHandler", "Going to add the listener to the set");

  dispatcher -> addReadFd(listener -> getFd());

  logTest("connectionHandler", "Going to enter the main loop");

  T_timestamp start = getTimestamp();
  T_timestamp now   = getTimestamp();

  int result;
  int fd;

  T_timestamp timeout;

  while (diffTimestamp(start, now) < 1000000000)
  {
    logTest("connectionHandler", "Going to wait for new events");

    timeout = nullTimestamp();

    result = dispatcher -> waitEvent(timeout);

    logTest("connectionHandler", "Dispatcher returned %d", result);

    while ((fd = dispatcher -> nextReadEvent()) != -1)
    {
      if ((fd = listener -> accept()) != -1)
      {
        requests++;

        logTest("connectionHandler", "Going to handle request %d",
                    requests);

        processHandler(fd);
      }
    }

    processCheck();

    now = getTimestamp();
  }

  logTest("connectionHandler", "Exiting with %ld seconds elapsed since start",
              diffTimestamp(start, now) / 1000);

  dispatcher -> removeReadFd(listener -> getFd());

  dispatcher -> end();

  delete dispatcher;

  logTest("connectionHandler", "Deleted the dispatcher");

  listener -> end();

  delete listener;

  logTest("connectionHandler", "Deleted the listener");

  processWait();

  return requests;
}

int processCheck()
{
  logTest("processCheck", "Going to check the exited processes");

  for (int i = 0; i < DEFAULT_PROCESS_ENTRIES; i++)
  {
    if (processes[i] != NULL)
    {
      if (processes[i] -> isRunning() == 0)
      {
        logTest("processCheck", "Process with pid %d has died",
                    processes[i] -> getPid());

        delete processes[i];

        processes[i] = NULL;
      }
      else
      {
        logTest("processCheck", "Process with pid %d is running",
                    processes[i] -> getPid());
      }
    }
  }

  return 1;
}

int processWait()
{
  logTest("processWait", "Going to wait for the running processes");

  for (int i = 0; i < DEFAULT_PROCESS_ENTRIES; i++)
  {
    if (processes[i] != NULL)
    {
      logTest("processWait", "Waiting for the process with pid %d",
                  processes[i] -> getPid());

      for (;;)
      {
        if (processes[i] -> wait() == 1)
        {
          delete processes[i];

          processes[i] = NULL;

          break;
        }
      }
    }
  }

  logTest("processWait", "All children processes exited");

  return 1;
}

int processHandler(int fd)
{
  logTest("processHandler", "Going to run a new process");

  for (;;)
  {
    for (int i = 0; i < DEFAULT_PROCESS_ENTRIES; i++)
    {
      if (processes[i] == NULL)
      {
        logTest("processHandler", "Starting process for descriptor %d", fd);

        processes[i] = new Process();

        processes[i] -> setFunction(requestHandler, NULL);

        processes[i] -> setStdin(fd);
        processes[i] -> setStdout(fd);
        processes[i] -> setStderr(fileno(stderr));

        processes[i] -> start();

        return 1;
      }
    }

    //
    // We were unable to find a slot.
    // Wait until a child exits.
    //

    logTest("processHandler", "No free slots found");

    processes[0] -> wait(getTimestamp(10));

    processCheck();
  }
}

int requestHandler(void *)
{
  logTest("requestHandler", "Going to handle the new request");

  for (int i = 0; i < DEFAULT_PROCESS_ENTRIES; i++)
  {
    if (processes[i] != NULL)
    {
      logTest("requestHandler", "Cleaning up process table");

      delete processes[i];

      processes[i] = NULL;
    }
  }

  logTest("requestHandler", "Cleaning up listener and dispatcher");

  delete listener;

  listener = NULL;

  delete dispatcher;

  dispatcher = NULL;

  char buffer[DEFAULT_BUFFER_LENGTH];

  //
  // Create the class for parsing and
  // handling the HTTP request.
  //

  Request *request = new Request();

/*
FIXME
*/
  request -> setRootPath("/home/pinzari/NX/apache_1.3.33/htdocs/manual");
/*
  request -> setRootPath("/usr/share/doc/HTML");
*/
  request -> setProgramPath("/home/pinzari/NX/nxproxy");
  request -> setProgramName("nxproxy");

  for (;;)
  {
    logTest("requestHandler", "Reading from descriptor %d",
                fileno(stdin));

/*
FIXME: This is temporarily used to dump the content of
       data sent by X clients at connection time.
*/

    char data[1024];

    int result;

    while ((result = read(0, data, 1024)) > 0)
    {
      logDump("requestHandler", data, result);
    }

    logTest("requestHandler", "Exiting");

    exit(0);

    if (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
      //
      // We are waiting for 1, when the request is
      // complete, or -1, in the case of an error.
      //

      if (request -> parse(buffer, strlen(buffer)) != 0)
      {
        break;
      }
    }
  }

  if (request -> isError() != request_error_none)
  {
    logTest("requestHandler", "Aborting request with error '%s'",
                request -> getErrorString());
  }
  else
  {
    logTest("requestHandler", "Executing request with URL '%s'",
                request -> getUrl());
  }

  //
  // Read from the file and write to the socket.
  //

  const char *name = request -> getFile();

  if (name != NULL)
  {
    int in  = open(name, O_RDONLY);

    if (in != -1)
    {
      int out = fileno(stdout);

      write(out, "HTTP/1.1 200 OK", strlen("HTTP/1.1 200 OK"));
      write(out, "Content-type: ", strlen("Content-Type: "));
      write(out, request -> getMimeString(),
                strlen(request -> getMimeString()));
      write(out, "\n\n", strlen("\n\n"));

      int result;

      while ((result = read(in, buffer, sizeof(buffer))) > 0)
      {
        write(fileno(stdout), buffer, result);
      }
    }
  }

  fclose(stdin);
  fclose(stdout);

  delete request;

  logTest("requestHandler", "Exiting from the request handler");

  return 1;
}












/*
FIXME

  Response *response = new Response();

  if (request -> isComplete() == 1)
  {
    logTest("requestHandler", "Got a complete HTTP request as '%s'",
                request -> getUrl());

    response -> parse(request -> getUrl());
  }
  else
  {
    logTest("requestHandler", "Error %d parsing the HTTP request",
                request -> getError());

    response -> parse(request -> getError());
  }

  //
  // Check if the response requires reading
  // from a file or running a CGI program.
  //

  if (response -> getHeader(buffer, sizeof(buffer)) == -1)
  {
    logTest("requestHandler", "Error preparing the "
                "header of the HTTP response");

    goto requestHandlerError;
  }
  else if (fputs(buffer, stdout) <= 0)
  {
    logTest("requestHandler", "Error %d writing the HTTP "
                "header, '%s'", EGET(), ESTR());

    goto requestHandlerError;
  }

  
%d parsing the HTTP request",

  if (response -> isFile() == 1)
  {
    if (response -> getHeader(buffer, sizeof(buffer)) == -1)
    {
      logTest("requestHandler", "Error preparing the header for the response
%d parsing the HTTP request",
                  request -> getError());
      

    if (fputs(bufferBegin, stdout) <= 0 ||

    
    int error = response -> getError();

    delete response;

    response = new Response(error);
  }
  else if (response -> isFile() == 1)
  {
    

      logTest("requestHandler", "Read buffer '%s' from input "
                  "descriptor %d", buffer, fileno(stdin));


        Response *response = new Response(request -> error());

        while (response -> write() > 0);

      if (strcmp(buffer, "\n") == 0 || strcmp(buffer, "\r\n") == 0)
      {
        logTest("requestHandler", "Received a complete request");

        break;
      }
    }
    else
    {
      logTest("requestHandler", "Nothing read from input "
                  "descriptor %d", fileno(stdin));

      break;
    }
  }

  char *bufferBegin =
"\
HTTP/1.1 200 OK\r\n\
Date: Fri, 06 May 2005 14:32:15 GMT\r\n\
Server: Apache/2.0.49 (Linux/SuSE)\r\n\
Connection: close\r\n\
Content-Type: text/html; charset=ISO-8859-1\r\n\
\r\n\
\r\n\
<html>\r\n\
<head>\r\n\
</head>\r\n\
<body>\r\n\
Hello world!\r\n\
</body>\r\n\
\r\n\
<p>\r\n\
\r\n\
";

  char *bufferEnd =
"\
\r\n\
<p>\r\n\
\r\n\
</html>\r\n\
\r\n\
";

  char bufferCount[128];

  sprintf(bufferCount, "This is request %d.\r\n", requests);

  for (;;)
  {
    logTest("requestHandler", "Writing to descriptor %d",
                fileno(stdout));

    if (fputs(bufferBegin, stdout) <= 0 ||
            fputs(bufferCount, stdout) <= 0 ||
                fputs(bufferEnd, stdout) <= 0)
    {
      logTest("requestHandler", "Writing to stdout failed "
                  "with error %d, '%s'", EGET(), ESTR());
    }

    break;
  }

  fclose(stdin);
  fclose(stdout);

  logTest("requestHandler", "Exiting from the request handler");

  return 1;
}

*/
