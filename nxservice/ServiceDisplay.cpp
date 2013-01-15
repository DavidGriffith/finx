/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2006, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXSERVICE, NX protocol compression and NX extensions to this software  */
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


#include <System.h>
#include <Logger.h>
#include <Display.h>

#include "ServiceDisplay.h"

#include <sys/un.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#ifdef __CYGWIN32__
extern "C" void cygwin_conv_to_full_posix_path(const char *path, char *posix_path);
extern "C" void cygwin_win32_to_posix_path_list(const char *win32, char *posix);
extern "C" int  cygwin_win32_to_posix_path_list_buf_size(const char *past_list);
#endif

#ifndef UNIX_PATH_MAX
#ifndef UNIX_PATH_LEN
#define UNIX_PATH_MAX 108
#else
#define UNIX_PATH_MAX UNIX_PATH_LEN
#endif
#endif

#define WARNING
#define PANIC
#define TEST
#undef  DEBUG

ServiceDisplay::ServiceDisplay(const char* path, int argc, char** argv, int nolisten)
  :Service(path, "display")
{
   resolution_ = "640x480";
   name_ = "NX";
   multiwindow_ = 0;
   multiplemonitors_ = 0;
   fullscreen_ = 0;
   fontserver_ = NULL;

   serverpath_ = NULL;
   authpath_ = NULL;
   display_ = NULL;

   timeout_ = 30;

   nolisten_ = nolisten;

   int n = 0;

   #ifdef __CYGWIN32__

   grabKeyboard_ = new char [11];
   disableDirectDraw_ = 0;

   for (n = 0; n < argc; n++)
   {
     if (strcmp(argv[n], "--name") == 0)
     {
       if (n+1 < argc) name_ = strdup(argv[n+1]);
     }
     
     if (strcmp(argv[n], "--fontserver") == 0)
     {
       if (n+1 < argc) fontserver_ = strdup(argv[n+1]);
     }
    
     else if (strcmp(argv[n], "--resolution") == 0)
     {
       if (n+1 < argc)
       {
	 if (strcmp(argv[n+1], "fullscreen") == 0)
	 {
	    fullscreen_ = 1;
	 }
	 else
	   resolution_ = strdup(argv[n+1]);
       }
     }
     else if (strcmp(argv[n], "--multiwindow") == 0)
     {
       if (n+1 < argc && strcmp(argv[n+1], "1") == 0)
       {
         multiwindow_ = 1;
       }
     }
     else if (strcmp(argv[n], "--multiplemonitors") == 0)
     {
       multiplemonitors_ = 1;
     }
     else if (strcmp(argv[n], "--keyhook") == 0)
     {
       strcpy(grabKeyboard_, "-keyhook");
     }
     else if (strcmp(argv[n], "--nokeyhook") == 0)
     {
       strcpy(grabKeyboard_, "-nokeyhook");
     }

     if (strcmp(argv[n], "--engine") == 0)
     {
       disableDirectDraw_ = 1;
     }
   }

   #else

   for (n = 0; n < argc; n++)
   {
     if (strcmp(argv[n], "--serverpath") == 0)
     {
       if (n+1 < argc) serverpath_ = strdup(argv[n+1]);
     }
     else if (strcmp(argv[n], "--auth") == 0)
     {
       if (n+1 < argc) authpath_ = strdup(argv[n+1]);
     }
     else if (strcmp(argv[n], "--display") == 0)
     {
       if (n+1 < argc) display_ = strdup(argv[n+1]);
     }
   }

   #endif
}

ServiceDisplay::~ServiceDisplay()
{
}

int ServiceDisplay::start()
{
  #ifdef __CYGWIN32__

  const char *systemPath = getSystemDir();
  char s_display[DEFAULT_STRING_LENGTH];
  int display = 0;

  //
  // FIXME: we do anything in case of failure
  //
  writeTypeFile();

  if (systemPath == NULL)
  {
    #ifdef WARNING
    logUser("Error: Cannot find NX system path. Exiting.");
    #endif

    return -1;
  }

  if ((strlen(systemPath) + strlen("/bin/") + strlen(DISPLAY_BIN) + 1) > DEFAULT_STRING_LENGTH)
  {
    #ifdef WARNING
    logUser("Error: nxwin path too long. Exiting.");
    #endif

    return -1;
  }

  strcpy(executablePath_, systemPath);
  strcat(executablePath_, "/bin/");
  strcat(executablePath_, DISPLAY_BIN);

  port_ = getFreePort(6000);

  if (port_ == 0)
  {
    #ifdef WARNING
    logUser("Error: Cannot find free port. Exiting.");
    #endif

    return -1;
  }

  display = getFreeDisplay();

  #ifdef TEST
  logUser("Info: Using display: %d", display);
  #endif


  char *authorityOSPath = getenv("XAUTHORITY");
  int len = cygwin_win32_to_posix_path_list_buf_size(authorityOSPath);
  char authorityPath[len];

  cygwin_win32_to_posix_path_list(authorityOSPath, authorityPath);


  #ifdef TEST
  logUser("Info: Using authority file: %s", authorityPath);
  #endif

  #define FONT_PATHS_MAX 10000

  char *fontPaths = new char[FONT_PATHS_MAX];

  if (fontserver_)
  {
    strncpy(fontPaths, fontserver_, FONT_PATHS_MAX);
  }
  else
  {
    const char *baseAndSecondFontPath[2]={"\\share\\fonts", "\\X11R6\\lib\\X11\\fonts"};

    //
    // FIXME: Is the DEFAULT_STRING_LENGTH too long (or short)
    //        for a Windows path?
    //

    char chkpath[DEFAULT_STRING_LENGTH];
    char posixPath[DEFAULT_STRING_LENGTH];
    char path[DEFAULT_STRING_LENGTH];

    DIR *pdir;
    struct dirent *pent;

    #ifndef _DIRENT_HAS_D_TYPE
    char buf[DEFAULT_STRING_LENGTH];
    struct stat dir;
    #endif

    int count = 0;


    fontPaths[0] = '\0';

    int i = 0;
    for (i = 0; i < 2; i++)
    {
      strncpy(chkpath, systemPath, DEFAULT_STRING_LENGTH);
      strncat(chkpath, baseAndSecondFontPath[i], DEFAULT_STRING_LENGTH);
      strncat(chkpath, "\\", DEFAULT_STRING_LENGTH);

      pdir = opendir(chkpath);
      if (pdir)
      {
        while ((pent=readdir(pdir)))
        {
          //
          // We are looking for real directories.
          //

          #ifdef _DIRENT_HAS_D_TYPE
          if (pent->d_type != DT_DIR)
          #else
          snprintf(buf, DEFAULT_STRING_LENGTH, "%s%s", chkpath, pent->d_name);

          if (lstat(buf, &dir) != 0 ||
                S_ISLNK(dir.st_mode) != 0 ||
                  S_ISDIR(dir.st_mode) == 0)
          #endif
          {
            continue;
          }

          if ((strcmp(pent->d_name, ".")!=0) && (strcmp(pent->d_name, "..")!=0))
          {
            if (count)
              strncat(fontPaths, ",", FONT_PATHS_MAX);
            strncpy(path, chkpath, DEFAULT_STRING_LENGTH);
            strncat(path, pent->d_name, DEFAULT_STRING_LENGTH);
            cygwin_win32_to_posix_path_list(path, posixPath);

            #ifdef TEST
            logUser("Posix path='%s' path='%s'", posixPath, path);
            #endif

            strncat(fontPaths, posixPath, FONT_PATHS_MAX);
            count++;
          }
        }
        closedir(pdir);
      }
    }

    //
    // Verify add on fonts installation.
    //

    const char *addonFonts[4] = { "NX Client Fonts 75dpi_is1",
                                  "NX Client Fonts 100dpi_is1",
                                  "NX Client Fonts misc_is1",
                                  "NX Client Fonts Others_is1" };
    const char *addonDirs[4][4] = { { "75dpi", "", "", "" },
                                    { "100dpi", "", "", "" },
                                    { "misc", "", "", "" },
                                    { "Speedo", "Type1", "encodings", "util" } };
    const char *preRegKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
    char *addonInstalled = NULL;
    char *addonLocation = NULL;
    char buffer[1024];

    for (int i = 0; i < 4; i++)
    {
      //
      // Check the registry to know if add on
      // fonts are installed.
      //

      strncpy(buffer, preRegKey, 1023);
      strncat(buffer, addonFonts[i], strlen(buffer) - strlen(addonFonts[i]));

      #ifdef DEBUG
      logUser("Info: Verifying registry key '%s'", buffer);
      #endif

      addonInstalled = getRegistryKey(buffer, "InstallLocation", HKEY_CURRENT_USER);

      if (*addonInstalled == '\0')
      {
        delete [] addonInstalled;

        addonInstalled = getRegistryKey(buffer, "InstallLocation", HKEY_LOCAL_MACHINE);

        if (*addonInstalled == '\0')
        {
          delete [] addonInstalled;

          continue;
        }
      }

      delete [] addonInstalled;

      #ifdef DEBUG
      logUser("Info: Add on font '%s' installed.", addonFonts[i]);
      #endif

      //
      // If add on fonts are installed, they're in
      // the NX Client directory, or in the default
      // installation path.
      //

      if (addonLocation == NULL)
      {
        snprintf(buffer, 1023, "%s%s", preRegKey, "nxclient_is1");

        addonLocation = getRegistryKey(buffer, "InstallLocation", HKEY_CURRENT_USER);

        if (*addonLocation == '\0')
        {
          delete [] addonLocation;

          addonLocation = getRegistryKey(buffer, "InstallLocation", HKEY_LOCAL_MACHINE);

          if (*addonLocation == '\0')
          {
            delete [] addonLocation;

            const char *pf = getenv("PROGRAMFILES");

            if (pf == NULL)
            {
              #ifdef WARNING
              logUser("Warning: Could not get add on fonts path.");
              #endif

              break;
            }

            int lLength = strlen(pf) +
                             strlen("\\NX Client for Windows\\") + 1;
            addonLocation = new char[lLength];

            snprintf(addonLocation, lLength,
                         "%s\\NX Client for Windows\\", pf);
          }
        }

        //
        // If we reach this point, we have the
        // nxclient install path. Append the
        // fonts location.
        //

        strncpy(buffer, addonLocation, 1023);

        delete [] addonLocation;

        addonLocation = new char[strlen(buffer) + strlen("share\\fonts\\") + 1];

        snprintf(addonLocation, strlen(buffer) + strlen("share\\fonts\\"),
                     "%sshare\\fonts\\", buffer);
      }

      #ifdef DEBUG
      logUser("Info: Verifying fonts in directory '%s'.", addonLocation);
      #endif

      struct stat dStats;

      for (int p = 0; p < 4; p++)
      {
        if (*addonDirs[i][p] == '\0')
        {
          continue;
        }

        //
        // We don't want to add twice the add on
        // fonts (they may have been found by
        // the previous fonts search). As a trick,
        // verify if the fonts directory name is
        // already in the font path.
        //

        if (strstr(fontPaths, addonDirs[i][p]) != NULL)
        {
          continue;
        }

        snprintf(buffer, 1023, "%s\\%s", addonLocation, addonDirs[i][p]);

        if (lstat(buf, &dStats) != 0 ||
                S_ISLNK(dStats.st_mode) != 0 ||
                    S_ISDIR(dStats.st_mode) == 0)
        {
          continue;
        }

        if (count)
          strncat(fontPaths, ",", FONT_PATHS_MAX);
        cygwin_win32_to_posix_path_list(buffer, posixPath);

        #ifdef TEST
        logUser("Posix path='%s' path='%s'", posixPath, buffer);
        #endif

        //
        // These paths are in long format. May
        // this be a problem?
        //

        strncat(fontPaths, posixPath, FONT_PATHS_MAX);
        count++;
      }
    }

    if (addonLocation != NULL)
    {
      delete [] addonLocation;
    }
  }

  #ifdef TEST
  logUser("Info: Using fontpath '%s'.", fontPaths);
  #endif
 
  snprintf(s_display, DEFAULT_STRING_LENGTH, ":%d", display);

  addArgument(executablePath_);
  addArgument("-nowinkill");
  addArgument("-clipboard");
  addArgument("-noloadxkb");
  addArgument("-agent");
  addArgument("-hide");
  addArgument("-noreset");
  addArgument("-auth");
  addArgument(authorityPath);
  addArgument("-nolisten");

  //
  // FIXME: We had problems with the service path
  // longer than 108 characters. This is a system
  // limit of the AF_UNIX socket type. To getting
  // around the problem, we select the TCP socket
  // in the place of UNIX, specifying the socket
  // to NOT use in the -nolisten option.
  //

  switch (nolisten_)
  {
    case AF_UNIX:
      addArgument("local");
      break;
    case AF_INET:
      addArgument("tcp");
      break;
    default:

      #ifdef WARNING
      logUser("Warning: Bad nolisten protocol %d. Using AF_INET.", nolisten_);
      #endif

      addArgument("tcp");
      break;
  }

  addArgument("-fp");
  addArgument(fontPaths);

  if (disableDirectDraw_)
  {
    addArgument("-engine");
    addArgument("1");
  }

  if (fullscreen_)
  {
    addArgument("-fullscreen");
  }
  else
  {
    addArgument("-screen");
    addArgument("0");

    if (strcmp(resolution_, "available") != 0)
    {
      addArgument(resolution_);
    }
  }

  if (multiwindow_)
  {
    addArgument("-multiwindow");
  }

  if (multiplemonitors_ == 1)
  {
    addArgument("-multiplemonitors");
  }

  addArgument(grabKeyboard_);
  addArgument("-name");
  addArgument(name_);
  addArgument(s_display);

  #else //__CYGWIN32__

  if (!serverpath_)
  {
    #ifdef WARNING
    logUser("Error: Path to X Apple server is empty.");
    #endif

    return -1;
  }

  if (!authpath_)
  {
    #ifdef WARNING
    logUser("Error: Path to authority file is empty.");
    #endif

    return -1;
  }

  if (!display_)
  {
    #ifdef WARNING
    logUser("Error: Display value is empty.");
    #endif

    return -1;
  }

  strncpy(executablePath_, serverpath_, DEFAULT_STRING_LENGTH);

  #ifdef TEST
  logUser("Info: Starting AppleXServer[%s] on display [%s]", executablePath_, display_);
  #endif

  addArgument(executablePath_);
  addArgument("-auth");
  addArgument(authpath_);
  addArgument(display_);

  #endif

  int ret = run();

  return (ret);
}

int ServiceDisplay::getFreeDisplay()
{
  //
  // The free display search depends on the
  // socket that the X Server is going to
  // use to listen incoming connections.
  //

  if (nolisten_ == AF_UNIX)
  {
    sockaddr_in tcpAddr;
    int basePort = 6000;
    int port;
    int sock;

    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    #ifdef TEST
    logUser("Info: Going to retrieve a free TCP display port.");
    #endif

    for (port = basePort; port < 65535; port++)
    {
      tcpAddr.sin_port = htons(port);

      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      {
        #ifdef WARNING
        logUser("Error: Could not create a valid socket: %s.", strerror(errno));
        #endif

        return -1;
      }

      if (bind(sock, (sockaddr *)&tcpAddr, sizeof(tcpAddr)) == -1)
      {
        close(sock);

        #ifdef WARNING

        if (errno != EADDRINUSE)
        {
          logUser("Error: Could not bind the port %d: %s.", port, strerror(errno));
        }

        #endif

        continue;
      }

      close(sock);

      break;
    }

    port -= basePort;

    if (port < 0 || port > 59535)
    {
      #ifdef WARNING
      logUser("Error: Could not find a free display port!");
      #endif

      return -1;
    }
    else
    {
      #ifdef TEST
      logUser("Info: There is no server running on display [:%d].", port);
      #endif

      return port;
    }
  }
  else
  {
    sockaddr_un xServerAddrUNIX;
    int xServerAddrFamily = AF_UNIX;
    xServerAddrUNIX.sun_family = AF_UNIX;

    #ifdef TEST
    logUser("Info: Going to retrieve a free UNIX display port.");
    #endif

    for (int d = 0; d < 100; d++)
    {
      //logUser("getFreeDisplay: %d", d);
      //for mac we have to use this:
      //QString displayPath = "/tmp/.X11-unix/X" + QString::number(d);
      //for windows:
      char displayPath[DEFAULT_STRING_LENGTH];
      char displayStr[DEFAULT_STRING_LENGTH];

      #ifdef __CYGWIN32__
      strncpy(displayPath, rootPath_, DEFAULT_STRING_LENGTH);
      #else
      strncpy(displayPath, "/tmp", DEFAULT_STRING_LENGTH);
      #endif
      strncat(displayPath, "/.X11-unix", DEFAULT_STRING_LENGTH);

      /*
       * let's try to create the directory
       */

      mkdir(displayPath, S_IRUSR|S_IWUSR|S_IXUSR);

      /*
       * let's check if we can find free display - this will be probably alway 1st one
       */

      strncat(displayPath, "/X", DEFAULT_STRING_LENGTH);

      snprintf(displayStr, DEFAULT_STRING_LENGTH, "%d", d);
      strncat(displayPath, displayStr, DEFAULT_STRING_LENGTH);

      #ifdef DEBUG
      logUser("Info: getFreeDisplay: path=%s", displayPath);
      #endif

      strncpy(xServerAddrUNIX.sun_path, displayPath, UNIX_PATH_MAX);

      sockaddr *xServerAddr = (sockaddr *) & xServerAddrUNIX;
      unsigned int xServerAddrLength = sizeof(sockaddr_un);

      //
      // Connect to the X server.
      //

      int xServerFd;

      xServerFd = socket(xServerAddrFamily, SOCK_STREAM, PF_UNSPEC);

      if (xServerFd < 0)
      {
        /*
         * It is serious problem we should quit
         */

        #ifdef PANIC
        logUser("PANIC! Failed to create the socket. Error is [%d], [%s].", errno, strerror(errno));
        #endif

        return -1;
      }

      int result = connect(xServerFd, xServerAddr, xServerAddrLength);

      if (result < 0)
      {
        /*
         * It means that there is no server running on that socket,
         * so we can start a new X server.
         */

        #ifdef TEST
        logUser("Info: There is no server running on display [:%i].", d);
        #endif

        close(xServerFd);
        return d;
      }
      else
      {
        /*
         * it means that the server is running so we should skip and use
         * another socket
         */

        #ifdef WARNING
        logUser("Error: Successfully connected to the X server on [:%i].", d);
        #endif
      }

      close(xServerFd);
    }
  }

  return -1;
}

int ServiceDisplay::checkStart()
{
  int ret = 1;

  #ifdef __CYGWIN32__
  //
  // The log entry searched should be
  // carefully choosen.
  //

  ret = logCheck(winlogPath_, "winBlockHandler - pthread_mutex_unlock () returned");

  #endif

  return ret;
}

char *ServiceDisplay::getRegistryKey(const char *key, const char *value, HKEY hKey)
{
  HKEY hResultKey;
  char lszValue[255];
  DWORD dwType = REG_SZ;
  DWORD dwSize = 255;
  LONG status;

  int max = 1024;
  char *keyValue;

  //
  // This must be freed by the caller.
  //

  keyValue = new char[max];
  *keyValue = '\0';

  status = RegOpenKeyEx(hKey, key, 0L, KEY_READ, &hResultKey);

  *lszValue = '\0';

  if (status == ERROR_SUCCESS)
  {
    status = RegQueryValueEx(hResultKey, value, NULL, &dwType, (LPBYTE) &lszValue, &dwSize);

    if (status == ERROR_SUCCESS)
    {
      strncpy(keyValue, lszValue, max - 1);
    }
  }

  RegCloseKey(hResultKey);
  RegCloseKey(hKey);

  return keyValue;
}

