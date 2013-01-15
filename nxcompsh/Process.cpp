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
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define PANIC
#define WARNING
#define TEST
#define DEBUG

#include "Misc.h"
#include "Logger.h"
#include "Timestamp.h"
#include "Process.h"

char *Process::nullStreamName_ = "/dev/null";

FILE *Process::nullStream_ = NULL;

//
// Retry the fork() n times waiting for a
// second after each attempt. This seems
// to patch the intermittent failures on
// Cygwin.
//

#define RETRY_FORK_LIMIT    30
#define RETRY_FORK_INTERVAL 1000

Process::Process()
{
  logTrace("Process::Process");

  pid_        = -1;
  status_     = -1;
  privileged_ = -1;

  for (int i = 0; i < 256; i++)
  {
    parameters_[i]  = NULL;
    environment_[i] = NULL;
  }

  nextEnvironment_ = 0;
  nextParameter_   = 0;

  function_ = NULL;

  in_  = -1;
  out_ = -1;
  err_ = -1;

  inStream_  = NULL;
  outStream_ = NULL;
  errStream_ = NULL;

  nullStream_ = NULL;
}

Process::~Process()
{
  logTrace("Process::~Process");

  //
  // Close all the descriptors.
  //

  end();

  if (nullStream_  != NULL)
  {
    logTest("Process::~Process", "Closing the null stream");

    fclose(nullStream_);

    nullStream_ = NULL;
  }

  //
  // Free memory from the heap.
  //

  if (function_ == NULL)
  {
    for (int i = 0; i < nextParameter_; i++)
    {
      delete [] parameters_[i];
    }
  }

  for (int i = 0; i < nextEnvironment_; i++)
  {
    delete [] environment_[i];
  }
}

int Process::setCommand(const char *command)
{
  logTrace("Process::setCommand");

  if (setValue(parameters_[0], command) &&
          setValue(parameters_[1], command))
  {
    nextParameter_ = 2;

    return 1;
  }

  return -1;
}

int Process::setFunction(int (*function)(void *), void *parameter)
{
  logTrace("Process::setFunction");

  if (function_ != NULL || parameters_[0] != NULL)
  {
    logError("Process::setFunction", ESET(EPERM));

    return -1;
  }

  function_ = function;

  parameters_[0] = (char *) parameter;

  nextParameter_ = 1;

  return 1;
}

int Process::setPrivileged(int value)
{
  logTrace("Process::setPrivileged");

  if (pid_ != -1)
  {
    logError("Process::setPrivileged", ESET(EPERM));

    return -1;
  }

  privileged_ = value;
  
  return 1;
}

int Process::setDescriptor(int &std, int fd)
{
  logTrace("Process::setDescriptor");

  if (pid_ != -1)
  {
    logError("Process::setDescriptor", ESET(EPERM));

    return -1;
  }

  std = fd;

  return 1;
}

FILE *Process::setDescriptorStream(int &fd, FILE *&stream, char *mode)
{
  logTrace("Process::setDescriptorStream");

  if ((stream = fdopen(fd, mode)) == NULL)
  {
    logError("Process::setDescriptorStream::fdopen", EGET());

    logTest("Process::setDescriptorStream", "Can't create "
                "stream for descriptor %d", fd);

    return getNullStream();
  }

  return stream;
}

FILE *Process::getNullStream()
{
  logTrace("Process::getNullStream");

  if (nullStream_ == NULL)
  {
    logTest("Process::getNullStream", "Creating a "
                "fake stream as '%s'", nullStreamName_);

    if ((nullStream_ = fopen(nullStreamName_, "a+")) == NULL)
    {
      logError("Process::getNullStream::fopen", EGET());

      logTest("Process::getNullStream", "Can't create "
                  "a fake stream as '%s'", nullStreamName_);
    }
  }

  return nullStream_;
}

int Process::addParameter(const char *parameter)
{
  logTrace("Process::addParameter");

  if (function_ != NULL)
  {
    logTest("Process::addParameter", "Can't add a parameter "
                "to a function");

    logError("Process::addParameter", ESET(EPERM));

    return -1;
  }
  else if (nextParameter_ < 2)
  {
    logTest("Process::addParameter", "Can't add a parameter "
                "without a command");

    logError("Process::addParameter", ESET(EPERM));

    return -1;
  }

  if (nextParameter_ < parametersLimit_)
  {
    if (setValue(parameters_[nextParameter_], parameter) > 0)
    {
      nextParameter_++;

      return 1;
    }
  }
  else
  {
    logTest("Process::addParameter", "No space left in the "
                "parameter table");

    logError("Process::addParameter", ESET(ENOMEM));
  }

  return -1;
}

int Process::addEnvironment(const char *environment)
{
  logTrace("Process::addEnvironment");

  if (nextEnvironment_ < environmentLimit_)
  {
    if (setValue(environment_[nextEnvironment_], environment) > 0)
    {
      nextEnvironment_++;

      return 1;
    }
  }
  else
  {
    logTest("Process::addEnvironment", "No space left in the "
                "environment table");

    logError("Process::addEnvironment", ESET(ENOMEM));
  }

  return -1;
}

int Process::isRunning()
{
  logTrace("Process::isRunning");

  if (pid_ < 0)
  {
    return 0;
  }

  int status;

  int options = WNOHANG | WUNTRACED;

  int result = waitpid(pid_, &status, options);

  return (parseStatus(result, status) <= 0);
}

int Process::isSuccess()
{
  logTrace("Process::isSuccess");

  if (status_ == -1)
  {
    logTest("Process::isSuccess", "Child %d is "
                "still running", pid_);

    logWarning("Process::isSuccess", ESET(EPERM));

    return 0;
  }

  if (WIFEXITED(status_))
  {
    return (WEXITSTATUS(status_) == 0);
  }

  return 0;
}

int Process::kill(int signal)
{
  logTrace("Process::kill");

  logTest("Process::kill", "Sending signal %d to process %d",
              signal, pid_);

  if (::kill(pid_, signal) < 0 && EGET() != ESRCH)
  {
    logError("Process::kill::kill", EGET());

    return -1;
  }

  return 1;
}

int Process::wait()
{
  logTrace("Process::wait");

  if (pid_ < 0)
  {
    logError("Process::wait", ESET(ECHILD));

    return -1;
  }

  int status;

  int options = WUNTRACED;

  int result = waitpid(pid_, &status, options);

  return parseStatus(result, status);
}

int Process::wait(const T_timestamp timeout)
{
  logTrace("Process::wait");

  if (pid_ < 0)
  {
    logError("Process::wait", ESET(ECHILD));

    return 1;
  }

  //
  // Wait for the process until the timeout.
  //

  int status;

  int options = WUNTRACED;

  setTimer(timeout);

  int result;

  if ((result = waitpid(pid_, &status, options)) == -1)
  {
    if (EGET() == EINTR)
    {
      logTest("Process::wait", "Timeout raised waiting "
                  "for pid %d", pid_);

      return 0;
    }
    else
    {
      logError("Process::wait", EGET());

      return -1;      
    }
  }

  resetTimer();

  result = parseStatus(result, status);

  return result;
}

int Process::parseStatus(int result, int status)
{
  logTrace("Process::parseStatus");

  if (result > 0)
  {
    if (WIFSTOPPED(status))
    {
      logTest("Process::parseStatus", "Child %d was stopped "
                  "with signal %d", pid_, (WSTOPSIG(status)));
    }
    else
    {
      #ifdef TEST

      if (WIFEXITED(status))
      {
        logTest("Process::parseStatus", "Child %d exited "
                    "with status %d", pid_, (WEXITSTATUS(status)));
      }
      else if (WIFSIGNALED(status))
      {
        logTest("Process::parseStatus", "Child %d died "
                    "because of signal %d", pid_, (WTERMSIG(status)));
      }

      #endif

      status_ = status;

      return 1;
    }
  }
  else if (result < 0)
  {
    if (EGET() == ECHILD)
    {
      //
      // The process died but we missed to
      // call the waitpid() at the time.
      //

      logWarning("Process::parseStatus::waitpid", EGET());

      return 1;
    }

    logError("Process::parseStatus::waitpid", EGET());

    return -1;
  }

  return 0;
}

int Process::exitStatus(int result)
{
  logTrace("Process::exitStatus");

  exit(result);
}

int Process::waitFork()
{
  int limit   = RETRY_FORK_LIMIT;
  int timeout = RETRY_FORK_INTERVAL;

  int pid;

  for (int i = 0; i < limit; i++)
  {
    logTest("Process::waitFork", "Trying at %s",
                strMsTimestamp());

    //
    // It could optionally try again only if the
    // error code is 11, 'Resource temporarily
    // unavailable'.
    //

    if ((pid = fork()) >= 0)
    {
      break;
    }
    else if (i < limit - 1)
    {
      logTest("Process::waitFork::fork", "Function "
                     "fork failed");

      logError("Process::waitFork::fork", EGET());


      logTest("Process::waitFork", "Trying again");

      usleep(timeout * 1000);
    }
  }

  if (pid <= 0)
  {
    logTest("Process::waitFork", "Returning at %s",
                strMsTimestamp());
  }

  return pid;
}

int Process::start()
{
  logTrace("Process::start");

  int childIn[2]  = { -1, -1 };
  int childOut[2] = { -1, -1 };
  int childErr[2] = { -1, -1 };

  //
  // We either have 2 parameters and this process
  // will exec() a new command, or we have one or
  // none and this process will yield control to
  // a function.
  //

  if (function_ == NULL && (parameters_[0] == NULL ||
          parameters_[1] == NULL))
  {
    logTest("Process::start", "Can't start the process "
                "without a command or function");

    logError("Process::start", ESET(EPERM));

    return -1;
  }

  #ifdef TEST

  if (function_ == NULL)
  {
    logTest("Process::start", "Executing command '%s'",
                parameters_[0]);

    for (int i = 1; i < parametersLimit_ &&
             parameters_[i] != NULL; i++)
    {
      logTest("Process::start", "Parameter [%d] is '%s'",
                  i, parameters_[i]);
    }
  }
  else
  {
    logTest("Process::start", "Executing function at %p",
                function_);

    logTest("Process::start", "Passing data as %p",
                parameters_[0]);
  }

  for (int i = 0; i < environmentLimit_ &&
           environment_[i] != NULL; i++)
  {
    logTest("Process::start", "Environment [%d] is '%s'",
                i, environment_[i]);
  }

  #endif

  //
  // Create the pipes that will be used to replace
  // the standard descriptors.
  //

  if ((in_ == -1 && pipe(childIn) != 0) ||
          (out_ == -1 && pipe(childOut) != 0) ||
              (err_ == -1 && pipe(childErr) != 0))
  {
    logError("Process::start::pipe", EGET());

    return -1;
  }

  //
  // The fork() on Cygwin can show intermittent
  // failures. In this case we try again after
  // some time.
  //

  #ifdef __CYGWIN32__

  switch (pid_ = waitFork())

  #else

  switch (pid_ = fork())

  #endif

  {
    case -1:
    {
      //
      // An error was encountered.
      //

      logError("Process::start::fork", EGET());

      if (in_ == -1)
      {
        close(childIn[0]);
        close(childIn[1]);
      }

      if (out_ == -1)
      {
        close(childOut[0]);
        close(childOut[1]);
      }

      if (err_ == -1)
      {
        close(childErr[0]);
        close(childErr[1]);
      }

      return -1;
    }
    case 0:
    {
      //
      // We are the child process.
      //

      logTest("Process::start", "Child running with pid %d", getpid());

      //
      // Drop the privileges.
      //

      if (privileged_ != 1)
      {
        logTest("Process::start", "Child dropping the permissions");

        setgid(getgid());
        setuid(getuid());
      }

      //
      // Let the input descriptor inherited from the
      // parent replace the standard descriptors. The
      // descriptor can be either the one set by the
      // parent or our end of the pipe we created be-
      // fore forking.
      //
      // Handle the standard input.
      //

      if (in_ == -1)
      {
        logTest("Process::start", "Child replacing pipe "
                    "%d and %d for input", childIn[0], childIn[1]);

        if (childIn[0] != 0)
        {
          dup2(childIn[0], 0);

          close(childIn[0]);
        }

        close(childIn[1]);
      }
      else if (in_ != 0)
      {
        logTest("Process::start", "Child replacing input %d", in_);

        dup2(in_, 0);

        if (in_ != out_ && in_ != err_)
        {
          close(in_);
        }
      }
      else
      {
        logTest("Process::start", "Child inherited input");
      }

      in_ = 0;

      //
      // Handle the standard output.
      //

      if (out_ == -1)
      {
        logTest("Process::start", "Child replacing pipe "
                    "%d and %d for output", childOut[0], childOut[1]);

        if (childOut[1] != 1)
        {
          dup2(childOut[1], 1);

          close(childOut[1]);
        }

        close(childOut[0]);
      }
      else if (out_ != 1)
      {
        logTest("Process::start", "Child replacing output %d", out_);

        dup2(out_, 1);

        if (out_ != err_)
        {
          close(out_);
        }
      }
      else
      {
        logTest("Process::start", "Child inherited output");
      }

      out_ = 1;

      //
      // Handle the standard error.
      //

      if (err_ == -1)
      {
        logTest("Process::start", "Child replacing pipe "
                    "%d and %d for error", childErr[0], childErr[1]);

        if (childErr[1] != 2)
        {
          dup2(childErr[1], 2);

          close(childErr[1]);
        }

        close(childErr[0]);
      }
      else if (err_ != 2)
      {
        logTest("Process::start", "Child replacing error %d", err_);

        dup2(err_, 2);

        close(err_);
      }
      else
      {
        logTest("Process::start", "Child inherited error");
      }

      err_ = 2;

      //
      // Let the pid be our own pid.
      //

      pid_ = getpid();

      logTest("Process::start", "Child has descriptors "
                  "%d, %d, %d and pid %d", in_, out_, err_, pid_);

      //
      // Set the new environment for the process.
      //

      for (int i = 0; i < environmentLimit_ &&
               environment_[i] != NULL; i++)
      {
        putenv(environment_[i]);
      }

      //
      // Either execute the requested command or
      // yield control to the function.
      //

      if (parameters_[1] != NULL)
      {
        if (execvp(parameters_[0], parameters_ + 1) == -1)
        {
          logTest("Process::start", "Child failed to execute the command");

          logError("Process::start::execvp", EGET());
        }

        exitStatus(-1);
      }
      else
      {
        int result = function_((void *) parameters_[0]);

        exitStatus(result);
      }
    }
    default:
    {
      //
      // We are the parent process.
      //

      logTest("Process::start", "Parent started child with pid %d", pid_);

      if (in_ == -1)
      {
        close(childIn[0]);

        in_ = childIn[1];
      }

      if (out_ == -1)
      {
        close(childOut[1]);

        out_ = childOut[0];
      }

      if (err_ == -1)
      {
        close(childErr[1]);

        err_ = childErr[0];
      }

      logTest("Process::start", "Parent using descriptors %d, %d, %d",
                  in_, out_, err_);

      return 1;
    }
  }
}

int Process::end()
{
  logTrace("Process::end");

  if (pid_ == -1)
  {
    return 0;
  }

  if (in_ != 0)
  {
    if (inStream_  != NULL)
    {
      logTest("Process::end", "Closing the input stream");

      fclose(inStream_);

      inStream_ = NULL;

      in_ = -1;
    }
    else if (in_ != -1)
    {
      logTest("Process::end", "Closing the input descriptor");

      close(in_);

      in_ = -1;
    }
  }
  else
  {
    logTest("Process::end", "Input is the standard descriptor");
  }

  if (out_ != 1)
  {
    if (outStream_  != NULL)
    {
      logTest("Process::end", "Closing the output stream");

      fclose(outStream_);

      outStream_ = NULL;

      out_ = -1;
    }
    else if (out_ != -1)
    {
      logTest("Process::end", "Closing the output descriptor");

      close(out_);

      out_ = -1;
    }
  }
  else
  {
    logTest("Process::end", "Output is the standard descriptor");
  }

  if (err_ != 2)
  {
    if (errStream_  != NULL)
    {
      logTest("Process::end", "Closing the error stream");

      fclose(errStream_);

      errStream_ = NULL;

      err_ = -1;
    }
    else if (err_ != -1)
    {
      logTest("Process::end", "Closing the error descriptor");

      close(err_);

      err_ = -1;
    }
  }
  else
  {
    logTest("Process::end", "Error is the standard descriptor");
  }

  return 1;
}
