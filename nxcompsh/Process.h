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

#ifndef Process_H
#define Process_H

#include "Timestamp.h"
#include "Runnable.h"

class Process : public Runnable
{
  public:

  Process();

  ~Process();

  int setCommand(const char *command);

  int addParameter(const char *parameter);

  int addEnvironment(const char *environment);

  int setFunction(int (*function)(void *), void *parameter);

  int isRunning();

  int isSuccess();

  int getPid()
  {
    return pid_;
  }

  int getStatus()
  {
    return status_;
  }

  int setPrivileged(int value);

  //
  // Assign descriptors to be used by the process
  // after it is started. When descriptors are not
  // assigned, the start() function will create a
  // set of pipes and will use them to replace the
  // standard input, output and error.
  //

  int setStdin(int fd)
  {
    return setDescriptor(in_, fd);
  }

  int setStdout(int fd)
  {
    return setDescriptor(out_, fd);
  }

  int setStderr(int fd)
  {
    return setDescriptor(err_, fd);
  }

  int getStdin()
  {
    return in_;
  }

  int getStdout()
  {
    return out_;
  }

  int getStderr()
  {
    return err_;
  }

  //
  // Return or create streams on demand
  // for the standard descriptors.
  //

  FILE *getStdinStream()
  {
    return (inStream_ != NULL ? inStream_ :
                setDescriptorStream(in_, inStream_, "w"));
  }

  FILE *getStdoutStream()
  {
    return (outStream_ != NULL ? outStream_ :
                setDescriptorStream(out_, outStream_, "r"));
  }

  FILE *getStderrStream()
  {
    return (errStream_ != NULL ? errStream_ :
                setDescriptorStream(err_, errStream_, "r"));
  }

  //
  // Start and manage the process at
  // run-time.
  //

  int start();

  int end();

  int kill(int signal);

  int wait();

  int wait(const T_timestamp timeout);

  protected:

  int setDescriptor(int &std, int fd);

  FILE *setDescriptorStream(int &fd, FILE *&stream, char *mode);

  static FILE *getNullStream();

  private:

  int parseStatus(int result, int status);

  int exitStatus(int result);

  int waitFork();

  static const int parametersLimit_  = 256;
  static const int environmentLimit_ = 256;

  int pid_;
  int status_;
  int privileged_;

  char *parameters_[parametersLimit_];
  char *environment_[environmentLimit_];

  int nextParameter_;
  int nextEnvironment_;

  int (*function_)(void *);

  int in_;
  int out_;
  int err_;

  FILE *inStream_;
  FILE *outStream_;
  FILE *errStream_;

  static char *nullStreamName_;

  static FILE *nullStream_;
};

#endif /* Process_H */
