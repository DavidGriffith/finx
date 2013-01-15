/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2005, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXKILL, NX protocol compression and NX extensions to this software     */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/

#ifndef CONTROL_H
#define CONTROL_H

class Control
{
  public:

  Control();
  ~Control();

  void  printHelp();
  void  printVersion();
  int   parseCommandLineOptions(int argc, const char** argv);

  char* getDotNXDirPath();
  char* getHomePath();

  bool  isListMode() const;
  bool  isKillMode() const;
  bool  isKillHard() const;
  bool  isHandleMode() const;
  bool  isEnvironmentMode() const;
  char* getSessionID();
  int   getPid() const;
  int   getSignal() const;

  protected:

  bool  listMode_;
  bool  killMode_;
  bool  killHard_;
  bool  handleMode_;
  bool  environmentMode_;
  char* id_;
  int   pid_;
  int   signal_;
  char* nxDirPath_;
  char* homePath_;
};

#endif //CONTROL_H

