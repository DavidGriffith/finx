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

int nxuexecDirNotWritableForEveryone(const char *dirname)
{
  struct stat dirstat;

  if (lstat (dirname, &dirstat) < 0)
  {
    fprintf(stderr, "Error: Cannot stat '%s'. Error is %d '%s'.\n",
                dirname, errno, strerror(errno));

    exit(1);
  }

  if ((S_ISDIR (dirstat.st_mode)) == 0)
  {
    fprintf(stderr, "Error: '%s' is not a directory.\n", dirname);

    exit(1);
  }

  if (S_IWOTH & dirstat.st_mode)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

int nxuexecDirExist(const char *dirname)
{
  struct stat dirstat;

  if (lstat (dirname, &dirstat) < 0)
  {
    fprintf(stderr, "Error: Cannot stat '%s'. Error is %d '%s'.\n",
                dirname, errno, strerror(errno));
    exit(1);
  }
  return 1;
}

int nxuexecScriptNotWritableForEveryone(const char *filename)
{
  struct stat filestat;

  if (lstat (filename, &filestat) < 0)
  {
     fprintf(stderr, "Error: Cannot stat '%s': Error is %d '%s'.\n",
                 filename, errno, strerror(errno));
     exit(1);
  }

  if (S_IWOTH & filestat.st_mode)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

int nxuexecScriptExist(const char *filename)
{
  struct stat filestat;

  if (lstat (filename, &filestat) == 0)
  {
    return 1;
  }
  else
  {
    fprintf(stderr, "Error: Cannot stat '%s': Error is %d '%s'.\n",
                filename, errno, strerror(errno));

    exit(1);
  }
}

int nxuexecScriptIsFile(const char *filename)
{
  struct stat filestat;

  if (lstat (filename, &filestat) < 0)
  {
    fprintf(stderr, "Error: Cannot stat '%s': Error is %d '%s'.\n",
                filename, errno, strerror(errno));

    exit(1);
  }
  else
  {
    if (S_ISREG(filestat.st_mode))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

ino_t getInode(const char *dirname)
{
  struct stat dirstat;

  if (stat (dirname, &dirstat) < 0)
  {
    fprintf(stderr, "Error: Cannot stat '%s': Error is %d '%s'.\n",
                dirname, errno, strerror(errno));
    exit(1);
  }

  return dirstat.st_ino;
}

int nxuexecCheckPath(const char *scriptName)
{
  char *dirc, *basec, *bname, *dname;
  dirc = strdup(scriptName);
  basec = strdup(scriptName);
  dname = dirname(dirc);

  if (strcmp(dname,".") != 0)
  {
    if (getInode(dname) != getInode(nxuexecSecureDirectory))
    {
      fprintf(stderr, "Error: Cannot run scripts from '%s' directory.\n",
                  dname);
      fprintf(stderr, "Error: The script has to be in '%s' directory.\n",
                  nxuexecSecureDirectory);

      exit(1);
    }
  }
  else
  {
    bname = basename(basec);

    if ((strlen(nxuexecSecureDirectory) + 1 + strlen(bname)) > MAX_LENGTH )
    {
      fprintf(stderr, "Error: Full path too long. More than '%d' for '%s/%s.\n",
                  MAX_LENGTH, nxuexecSecureDirectory, bname);

      exit(1);
    }

    strcpy(nxuexecCorrectScriptName, nxuexecSecureDirectory);
    strcat(nxuexecCorrectScriptName, "/");
    strcat(nxuexecCorrectScriptName, bname);
  }

  return 1;
}

void nxuexecCheckArgs(int argc, char *argv[])
{
  int x;

  for(x = 0; x < argc; x++)
  {
    if (strchr(argv[x],'\''))
    {
      fprintf(stderr, "Error: Wrong character '\'' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],';'))
    {
      fprintf(stderr, "Error: Wrong character ';' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'&'))
    {
      fprintf(stderr, "Error: Wrong character '&' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'|'))
    {
      fprintf(stderr, "Error: Wrong character '|' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'>'))
    {
      fprintf(stderr, "Error: Wrong character '>' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'<'))
    {
      fprintf(stderr, "Error: Wrong character '<' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'*'))
    {
      fprintf(stderr, "Error: Wrong character '*' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'?'))
    {
      fprintf(stderr, "Error: Wrong character '?' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'`'))
    {
      fprintf(stderr, "Error: Wrong character '`' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

     if (strchr(argv[x],'$'))
    {
      fprintf(stderr, "Error: Wrong character '$' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'('))
    {
      fprintf(stderr, "Error: Wrong character '(' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],')'))
    {
      fprintf(stderr, "Error: Wrong character ')' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'{'))
    {
      fprintf(stderr, "Error: Wrong character '{' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'}'))
    {
      fprintf(stderr, "Error: Wrong character '}' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'['))
    {
      fprintf(stderr, "Error: Wrong character '[' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],']'))
    {
      fprintf(stderr, "Error: Wrong character ']' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'!'))
    {
      fprintf(stderr, "Error: Wrong character '!' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }

    if (strchr(argv[x],'#'))
    {
      fprintf(stderr, "Error: Wrong character '#' in argument: '%s'.\n",
                  argv[x]);
      exit(1);
    }
  }

  return;
}

char *nxuexecGetUser()
{
  uid_t uid;
  struct passwd *pw;

  uid = getuid();
  pw = getpwuid(uid);

  return pw->pw_name;
}
