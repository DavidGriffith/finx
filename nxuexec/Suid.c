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

#include "Main.h"

int nxuexecScriptSuided(const char *filename)
{
  struct stat filestat;

  if (stat(filename, &filestat) < 0)
  {
    fprintf(stderr, "Error: Cannot stat '%s': Error is %d '%s'.\n",
                filename, errno, strerror(errno));

    exit(1);
  }
  else
  {
    if (S_ISUID & filestat.st_mode)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

int nxuexecScriptSgided(const char *filename)
{
  struct stat filestat;

  if (stat(filename, &filestat) < 0)
  {
    fprintf(stderr, "Error: Cannot stat '%s': Error is %d '%s'.\n",
                filename, errno, strerror(errno));

    exit(1);
  }
  else
  {
    if (S_ISGID & filestat.st_mode)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

uid_t nxuexecGetOwnerUid(const char *filename)
{
  struct stat filestat;

  if (stat (filename,&filestat) < 0)
  {
    fprintf(stderr,"Error: Cannot stat '%s': Error is %d '%s'.\n",
                filename, errno, strerror(errno));

    exit(1);
  }
  else
  {
    return filestat.st_uid;
  }
}

gid_t nxuexecGetOwnerGid(const char *filename)
{
  struct stat filestat;
  if (stat (filename, &filestat) < 0)
  {
    fprintf(stderr, "Error: Cannot stat '%s': Error is %d '%s'.\n",
                filename, errno, strerror(errno));

    exit(1);
  }
  else
  {
    return filestat.st_gid;
  }
}

