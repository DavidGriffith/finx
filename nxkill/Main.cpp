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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>


#include "Control.h"

#ifdef  __CYGWIN32__
#include <sys/cygwin.h>
#include <windows.h>
#endif

#undef TEST
#undef DEBUG
#undef WARNING
#undef PANIC

//
// Parses session name returning: name, display and id.
// type of the session is returned as char - if returned
// value is 0 - it means that this is not correct session dir.
// In other cases it returns the type of session "C" or "S".
//

char parseSessionName(const char* session, char*& name, char*& display, char*& id)
{
  int type = 0;
  char* str = 0;
  int len = 0;

  if (strncmp(session, "S-", 2) == 0)
  {
    type = 'S';
  }
  else if (strncmp(session, "C-", 2) == 0)
  {
    type = 'C';
  }
  else
  {
    return 0;
  }

  str = rindex(session,'-');

  id = strdup(str + 1);

  while (((*(--str)) != '-') && (str != session))
  {
    len++;
  }

  display = new char[len+1];

  display[len] = '\0';

  strncpy(display,str+1,len);

  if ((len = (str - session - 2)) <= 0)
  {
    return 0;
  }

  name = new char[len+1];

  name[len] = '\0';

  strncpy(name, session + 2, len);

  return type;
}


//
// Shows details of the session with the given
// session id.
//

int getProxyPid(const char* dotNXDir, const char* session)
{
  char *path = new char[strlen(dotNXDir) + strlen(session) + strlen("session") + 3];
  const int BUF_SIZE = 80;
  char buf[BUF_SIZE];
  bool found = false;
  int  pid = 0;

  strcpy(path,dotNXDir);
  strcat(path,"/");
  strcat(path,session);

  #ifdef DEBUG
  printf("Debug: showSessionDetails: sessionDir = [%s].\n",path);
  #endif

  DIR* sessionDir = opendir(path);

  if (sessionDir == NULL)
  {
    perror("Cannot open session directory");
    return -1;
  }

  closedir(sessionDir);

  strcat(path,"/session");

  FILE* sessionFile = fopen(path,"r");

  if (sessionFile == NULL)
  {
    perror("Cannot open session file");
    return -1;
  }

  while (!found && (fgets(buf,BUF_SIZE,sessionFile) != NULL))
  {
    if (strncmp(buf,"Info: Proxy running in client mode with pid '",
                strlen("Info: Proxy running in client mode with pid '")) == 0)
    {
      pid = atoi(buf + strlen("Info: Proxy running in client mode with pid '"));
      found = true;
    }
  }
  return pid;
}


int showSessionDetails(const char* dotNXDir, const char* session)
{
  int pid = getProxyPid(dotNXDir,session);

  if (pid > 0)
  {
    //
    // We've got the right pid
    //
    printf("P %d\n",pid);
    return 0;
  }
  else
  {
    //
    // We didn't succed to parse the session file
    //
    printf("Error: cannot get the nxproxy pid!\n");
  }
  return 0;
}

//
// Finds a session with a given id. Returns the whole
// session name in the form [S,C]-SESSION_NAME-DISPLAY-SESSIONID
// if the session is not found in the given dir - returns NULL.
//

char* findSession(const char* dotNXDir, char* sessionId)
{
  DIR  *nxDir = opendir(dotNXDir);
  char *session = NULL;
  struct dirent* dir = NULL;
  bool found = false; //needed to get out of the while when looking for specific sessionID

  if (nxDir == NULL)
  {
    perror("Cannot open .nx directory");
    return NULL;
  }

  while ((!found) && ((dir = readdir(nxDir)) != NULL))
  {
    session = dir -> d_name;

    if ((strncmp(session, "S-", 2) == 0) || (strncmp(session, "C-", 2) == 0))
    {
      char *display     = NULL;
      char *name   = NULL;
      char *id = NULL;

      #ifdef DEBUG
      printf("Debug: session dir: [%s]\n", session);
      #endif

      if ((parseSessionName(session, name, display, id)) != 0)
      {
        if (sessionId != NULL)
        {
          if (strcmp(sessionId,id) == 0)
          {
            found = true;
          }
        }
      }

      if (name) delete name;
      if (display) delete display;
      if (id) delete id;
    }

  }

  closedir(nxDir);

  if(!found)
  {
    return NULL;
  }
  else
  {
    return session;
  }
}

//
// Kills the session with the given ID. It simply kills nxproxy
// that is used by that session.
//

int killSession(const char* dotNXDir, char* id)
{
  char* session = findSession(dotNXDir, id);

  if (session != NULL)
  {
    int pid = 0;

    if ((pid = getProxyPid(dotNXDir,session)) > 0)
    {
      if (kill(pid,SIGTERM))
      {
        perror("Error killing nxproxy");
        return -1;
      }
      else
      {
        return 0;
      }
    }
  }
  return -1;
}

void fillspace(char *str,unsigned int len)
{
  str[len-1] = '\0';

  for(unsigned int i = strlen(str); i < (len-1); i++)
  {
    str[i] = ' ';
  }
}



//
// listSession is listing the sessions running in the system
// if session
//

int listSessions(const char* dotNXDir, const char* sessionId)
{
  DIR* nxDir = opendir(dotNXDir);
  struct dirent* dir = NULL;
  bool found = false; //needed to get out of the while when looking for specific sessionID

  if (nxDir == NULL)
  {
    perror("Cannot open .nx directory");
    return -1;
  }

  printf("Type Id                               Display Host\n");
  printf("---- -------------------------------- ------- ------------------------------\n");

  while ((!found) && ((dir = readdir(nxDir)) != NULL))
  {
    char* session = dir -> d_name;

    if ((strncmp(session, "S-", 2) == 0) || (strncmp(session, "C-", 2) == 0))
    {
      char *name = NULL;
      char *display = NULL;
      char *id = NULL;
      char type = 0;

      #ifdef DEBUG
      printf("Debug: session dir: [%s]\n", session);
      #endif

      if ((type = parseSessionName(session, name, display, id)) != 0)
      {
        if (sessionId != NULL)
        {
          if (strcmp(sessionId,id) == 0)
          {
            showSessionDetails(dotNXDir,session);
            found = true;
            break;
          }
        }
        else
        {
          char str_type[5];
          char str_id[33];
          char str_display[8];
          char str_host[31];

          snprintf(str_type, 5, "%c", type);
          fillspace(str_type, 5);

          snprintf(str_id, 33, "%s", id);
          fillspace(str_id, 33);

          snprintf(str_display, 8, "%s", display);
          fillspace(str_display, 8);

          snprintf(str_host, 31, "%s",name);
          fillspace(str_host, 31);

          printf("%s %s %s %s\n", str_type, str_id, str_display, str_host);
        }
      }
      else
      {
        #ifdef DEBUG
        printf("Debug: session dir name is not correct!\n");
        #endif
      }

      if (name) delete name;
      if (display) delete display;
      if (id) delete id;
    }
  }

  closedir(nxDir);

  if (sessionId != NULL && !found)
  {
    printf("Error: session ['%s'] not found!\n",sessionId);
    return -1;
  }

  return 0;
}

#ifdef __CYGWIN32__
void setMountInRegistry(char* keyName, char* value)
{
    HKEY newKey;
    HKEY baseKey = HKEY_CURRENT_USER;
    DWORD newType;
    int rv = RegCreateKeyEx (baseKey, keyName, 0, (char *) "", REG_OPTION_NON_VOLATILE,
			                 KEY_ALL_ACCESS, 0, &newKey, &newType);
	#ifdef DEBUG		                 
    if (rv != ERROR_SUCCESS)
    {
      char *buf;
      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER
		             | FORMAT_MESSAGE_FROM_SYSTEM,
		             0, rv, 0, (CHAR *) & buf, 0, 0);
      fprintf (stderr, "Error (%ld): %s\n", rv, buf);
    }


    if (newType == REG_OPENED_EXISTING_KEY)
	  printf ("Key %s already exists\n", keyName);
    else
	  printf ("Key %s created\n", keyName);
	#endif

    rv = RegSetValueEx (newKey, "native", 0, REG_SZ, (const BYTE *) value, strlen (value) + 1);
    
    #ifdef DEBUG
    if (rv != ERROR_SUCCESS)
    {
      char *buf;
      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER
		             | FORMAT_MESSAGE_FROM_SYSTEM,
		             0, rv, 0, (CHAR *) & buf, 0, 0);
      fprintf (stderr, "Error (%ld): %s\n", rv, buf);
    }
    #endif
}
#endif

int listEnvironment()
{
  char* notSet = "not set";
  char* varH;
  char* varR;
  char* varS;
  char* varT;
  char* varF;
  char* nxFontPath;
  char* nxTmpPath;
  
  if ((varH = getenv("NX_HOME")) == NULL)
  {
    varH = notSet;
  }
  
  if ((varR = getenv("NX_ROOT")) == NULL)
  {
    varR = notSet;
  }
 
  if ((varS = getenv("NX_SYSTEM")) == NULL)
  {
    varS = notSet;
  } 
  
  
  DIR *dir = opendir("/tmp1");

  if (dir != NULL)
  {
  	//
  	// Mount point exists.
  	//
  	closedir(dir);
  	nxTmpPath = "/tmp";
  	
  	#ifdef __CYGWIN32__
  	varT = new char[MAX_PATH+1];
  	cygwin_conv_to_full_win32_path(nxTmpPath, varT);
  	#else
  	varT = nxTmpPath;
  	#endif 
  	
  }
  else if((nxTmpPath = getenv("NX_TEMP")) == NULL)
  {
  	//
  	// Mount point don't exists and NX_TEMP is not set.
  	// In this case we can't determine font location.
  	//
    varT = notSet;	
  }
  else
  {
  	varT = nxTmpPath;
    
    #ifdef __CYGWIN32__
    setMountInRegistry("Software\\Cygnus Solutions\\Cygwin\\mounts v2\\/tmp", varT);
  	#endif 
  }  
  
  dir = opendir("/usr/X11R6/lib/X11/fonts1");

  if (dir != NULL)
  {
  	//
  	// Mount point exists.
  	//
  	closedir(dir);
  	nxFontPath = "/usr/X11R6/lib/X11/fonts";
  	
  	#ifdef __CYGWIN32__
  	varF = new char[MAX_PATH+1];
  	cygwin_conv_to_full_win32_path(nxFontPath, varF);
  	#else
  	varF = nxFontPath;
  	#endif 
  	
  }
  else if(strcmp (varS, notSet) == 0)
  {
  	//
  	// Mount point don't exists and NX_SYSTEM is not set.
  	// In this case we can't determine font location.
  	//
    varF = notSet;	
  }
  else
  {
    nxFontPath = new char[strlen(varS) + strlen("/X11R6/lib/X11/fonts") + 1];
      
    strcpy(nxFontPath, varS);
    strcat(nxFontPath, "/X11R6/lib/X11/fonts");	
     
    dir = opendir(nxFontPath);
    
    if (dir != NULL)
    {
      //mountowanie katalogu
      #ifdef __CYGWIN32__
  	  varF = new char[MAX_PATH+1];
  	  cygwin_conv_to_full_win32_path(nxFontPath, varF);
      setMountInRegistry("Software\\Cygnus Solutions\\Cygwin\\mounts v2\\/usr/X11R6/lib/X11/fonts", varF);
      #else
      varF = nxFontPath;
      #endif 	
    }
    else
    {
      varF = notSet;
    }
  }

  printf("H %s\n", varH);
  printf("R %s\n", varR);
  printf("S %s\n", varS);
  printf("T %s\n", varT);
  printf("F %s\n", varF);

  return 0;
}

int main(int argc, const char** argv)
{
  Control control;

  //
  // parseCommandLineOptions returns
  // 0  - everything ok
  // 1  - everything ok - but we need to quit (help or version option
  //      was specified and already handled in inside parsing method
  // -1 - wrong parameters we should exit
  //

  int ret = control.parseCommandLineOptions(argc, argv);

  if (ret != 0)
  {
    exit(ret);
  }

  if (control.isListMode())
  {
    listSessions(control.getDotNXDirPath(),control.getSessionID());
  }
  else if (control.isKillMode())
  {
    if (control.getPid() != -1)
    {
      if (kill(control.getPid(), control.getSignal()) != 0)
      {
        perror("Error: Cannot kill PID");
      }
    }
    else
    {
      if (killSession(control.getDotNXDirPath(),control.getSessionID()))
      {
        printf("Error: Cannot kill session '%s'.\n",control.getSessionID());
      }
    }
  }
  else if (control.isHandleMode())
  {
    if (control.getPid() != -1)
    {
      if (kill(control.getPid(), control.getSignal()) != 0)
      {
        perror("Error: Cannot kill PID");
      }
    }
  }
  else if (control.isEnvironmentMode())
  {
  	//
  	//listEnvironment(control.getDotNXDirPath());
  	//
  	listEnvironment();
  }
  exit(0);
}
