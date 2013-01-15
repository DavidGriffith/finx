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

#ifndef System_H
#define System_H

extern int setSystemDir(const char *path);
extern int setRootDir(const char *path);
extern int setHomeDir(const char *path);

//
// getSystemDir returns the placement of NX system
// directory (the directory where we can find all
// libraries and NX components). It should be specified
// in $NX_SYSTEM environment variable
//
extern const char *getSystemDir();

//
// getRootDir returns the placement of .nx/
// that should be pointed by NX_ROOT environment
// variable
//
extern const char *getRootDir();

//
// getHomeDir returns the placement of users home
// directory that should be pointed by $NX_HOME
// environment variable
//
extern const char *getHomeDir();

#endif /* System_H */
