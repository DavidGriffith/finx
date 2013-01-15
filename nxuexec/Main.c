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

char nxuexecPathToDir [255];
char nxuexecSecureDirectory [255];
char nxuexecCorrectScriptName [255];

int main (int argc,char *argv[], char **envp)
{
  uid_t executeUid;
  gid_t executeGid;
  int result;
  char *scriptname;
  char *username;

  if (argc <= 1)
  {
    fprintf(stderr, "Usage: ./nxuexec [Option]\n\n");
    fprintf(stderr, "Run the privileged script specified as option. The script is\n");
    fprintf(stderr, "searched for in the '/usr/NX/scripts/restricted' directory.\n");

    exit(1);
  }

  nxuexecPathToDir[0] = 0;

  nxuexecSecureDirectory[0] = 0;

  nxuexecCorrectScriptName[0] = 0;

  nxuexecCheckArgs(argc, argv);

  username = nxuexecGetUser();

  #ifdef DEBUG
  fprintf(stdout, "Info: Setting '%s' for NX scripts directory.\n", PRENXSDIR);
  #endif

  if (strlen(PRENXSDIR) < 254)
  {
    strcpy(nxuexecSecureDirectory, PRENXSDIR);
  }
  else
  {
    fprintf(stderr, "Error: path '%s' is too long.\n", PRENXSDIR);

    exit(1);
  }

  if (nxuexecDirNotWritableForEveryone(nxuexecSecureDirectory) == 0)
  {
    fprintf(stderr, "Error: Directory '%s' is writable for everyone.\n",
                nxuexecSecureDirectory);

    exit(1);
  }

  scriptname = argv[1];

  if ((nxuexecCheckPath(scriptname)) == 0)
  {
    fprintf(stderr, "Error: Filename or path not correct '%s'.\n",
                scriptname);

    exit(1);
  }

  if (nxuexecCorrectScriptName[0] != 0)
  {
    scriptname = nxuexecCorrectScriptName;
  }

  if ((nxuexecScriptExist(scriptname)) == 0)
  {
    fprintf(stderr, "Error: Cannot open '%s':\n%s.\n", scriptname,
                strerror(errno));

    exit(1);
  }

  if ((nxuexecScriptIsFile(scriptname)) == 0)
  {
    fprintf(stdout, "Error: '%s' is not a regular file.\n", scriptname);

    exit(1);
  }

  if ((nxuexecScriptNotWritableForEveryone(scriptname)) == 0)
  {
    fprintf(stderr, "Error: File '%s' is writable for everyone.\n",
                scriptname);

    exit(1);
  }

  if ((nxuexecScriptIsTextScript(scriptname)) == 0)
  {
    fprintf(stderr, "Error: File '%s' is not a text script.\n", scriptname);

    exit(1);
  }

  #ifdef DEBUG
    fprintf(stdout, "Info: Runnig with: uid[%d] euid[%d] gid[%d] egid[%d].\n",
                getuid(), geteuid(), getgid(), getegid());
  #endif

  if ((nxuexecScriptSuided(scriptname)) == 1)
  {
    executeUid = nxuexecGetOwnerUid(scriptname);

    #ifdef DEBUG
    fprintf(stdout, "Info: Script is suided. Set executeUid for executions on '%d'.\n",
                executeUid);
    #endif
  }
  else
  {
    executeUid = getuid();

    #ifdef DEBUG
    fprintf(stdout, "Info: Script is not suided. Set executeUid for executions on '%d'.\n",
                executeUid);
    #endif
  }

  if ((nxuexecScriptSgided(scriptname)) == 1)
  {
    executeGid = nxuexecGetOwnerGid(scriptname);

    #ifdef DEBUG
    fprintf(stdout, "Info: Script have group id set. Set executeGid for executions on '%d'.\n",
                executeGid);
    #endif
  }
  else
  {
    executeGid = getgid();

    #ifdef DEBUG
    fprintf(stdout, "Info: Script is not guided. Set executeGid for executions on '%d'.\n",
                executeGid);
    #endif
  }

  #ifdef DEBUG
  fprintf(stdout,"Info: Executing with: uid [%d] gid [%d] \n", executeUid, executeGid);
  #endif

  result = nxuexecExecute(scriptname, executeUid, executeGid, argc, argv, username);

  result = result >> 8;

  #ifdef DEBUG
  fprintf(stderr, "nxuexec: Terminating with result '%d'.\n", result);
  #endif

  exit(result);
}

int nxuexecExecute (const char *scriptname, uid_t executeUid, gid_t executeGid, int argc, char *argv[], char *username)
{
  extern char **environ;
  /* char *argv[3]; */
  int x;
  char ppid[20];
  char **nargv;

  nargv = (char **) malloc(sizeof(char *[argc + 2]));

  snprintf(ppid, sizeof(ppid), "%d", getppid());

  if (scriptname == NULL)
  {
    fprintf(stderr, "Error: scriptname cannot be NULL.\n");

    return 1;
  }

  /*
   * pid = fork();
   * if (pid == -1) {
   *   fprintf(stderr,"Error: Cannot fork: '%s'.\n",strerror(errno));
   *   return -1;
   * }

   * if (pid == 0) {
   */

  setgid(executeGid);

  if (errno == -1)
  {
    fprintf(stderr, "Error: Cannot set setgid '%d': %s.\n", (int) executeGid,
                strerror(errno));
    exit(1);
  }

  #ifdef DEBUG
  fprintf(stdout, "Info: executive gid set to '%d'.\n", (int) executeGid);
  fprintf(stdout, "Info: real gid [%d] egid [%d].\n", getgid(), getegid());
  #endif

  setuid(executeUid);

  if (errno == -1)
  {
    fprintf(stderr, "Error: Cannot set uid '%d': %s.\n", (int) executeUid,
                strerror(errno));
    exit(1);
  }

  #ifdef DEBUG
  fprintf(stdout, "Info: executive uid set to '%d'.\n", executeUid);
  fprintf(stdout, "Info: real uid [%d] euid [%d].\n", getuid(),
              geteuid());
  fprintf(stdout, "Info: real gid [%d] egid [%d].\n", getgid(),
              getegid());
  #endif

  putenv("PATH=''");
  putenv("IFS=' \t\n'");
  #ifdef __sun
  putenv("CDPATH=");
  putenv("ENV=");
  putenv("BASH_ENV=");
  putenv("LD_LIBRARY_PATH=");
  putenv("LD_PRELOAD=");
  putenv("LD_AOUT_LIBRARY_PATH=");
  putenv("LD_AOUT_PRELOAD=");
  putenv("LD_KEEPDIR=");
  #else
  unsetenv("CDPATH");
  unsetenv("ENV");
  unsetenv("BASH_ENV");
  unsetenv("LD_LIBRARY_PATH");
  unsetenv("LD_PRELOAD");
  unsetenv("LD_AOUT_LIBRARY_PATH");
  unsetenv("LD_AOUT_PRELOAD");
  unsetenv("LD_KEEPDIR");
  #endif

  nargv[0] = (char *) scriptname;

  if(argc > 1)
  {
    nargv[1] = username;
    nargv[2] = ppid;

    for(x = 2; x < argc; x++)
    {
      nargv[x + 1] = argv[x];
    }

    nargv[x + 1] = NULL;
  }
  else
  {
    nargv[1] = NULL;
  }

  execve ((char *) scriptname, nargv, NULL);

  free(nargv);

  if (errno != 0)
  {
    fprintf(stderr, "Error: Cannot execute '%s': %s.\n", scriptname,
                strerror(errno));

    exit(errno);
  }

  #ifdef DEBUG
  fprintf(stderr, "Terminating with 0.\n", errno, strerror(errno));
  #endif

  exit(0);
}
