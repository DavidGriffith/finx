/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com.          */
/*                                                                        */
/* NXUEXEC, NX protocol compression and NX extensions to this software    */
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

#ifndef NXUEXEC_H
#define NXUEXEC_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pwd.h>

#define EGET()  (errno)
#define CONFIGFILE "/usr/NX/etc/node.cfg"
#define DIRCONFIGFIELD "NXSDIR"
#define PRENXSDIR "/usr/NX/scripts/restricted"
#define NXUEXEC_VERSION "3.5.0-1"
#define MAX_LENGTH 255
#define progname "nxuexec"

#if     defined(DEBUG)
#undef  DEBUG
#endif

ssize_t nxuexecGetLine(char **bufferp, size_t *leng, FILE *f);
int     nxuexecDirNotWritableForEveryone(const char *dirname);
int     nxuexecScriptNotWritableForEveryone(const char *filename);
int     nxuexecScriptExist(const char *filename);
int     nxuexecScriptIsTextScript(const char *filename);
int     nxuexecScriptSuided(const char *filename);
int     nxuexecScriptSgided(const char *filename);
int     nxuexecScriptIsFile(const char *filename);
int     nxuexecDirExist(const char *dirname);
int     nxuexecCheckPath(const char *scriptname);
int     nxuexecSetNXSDir();
uid_t   nxuexecGetOwnerUid(const char *filename);
gid_t   nxuexecGetOwnerGid(const char *filename);
void    nxuexecCheckArgs(int argc, char *argv[]);
char   *nxuexecGetUser();
int     nxuexecExecute (const char *scriptname, uid_t executeUid,
                            gid_t executeGid, int argc, char *argv[],
                            char *username);

extern char nxuexecPathToDir [255];
extern char nxuexecSecureDirectory [255];
extern char nxuexecCorrectScriptName [255];

#endif /* NXUEXEC_H */

