/* $Xorg: ddxLoad.c,v 1.3 2000/08/17 19:53:46 cpqbld Exp $ */
/************************************************************
Copyright (c) 1993 by Silicon Graphics Computer Systems, Inc.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Silicon Graphics not be 
used in advertising or publicity pertaining to distribution 
of the software without specific prior written permission.
Silicon Graphics makes no representation about the suitability 
of this software for any purpose. It is provided "as is"
without any express or implied warranty.

SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SILICON
GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, 
DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/
/* $XFree86: xc/programs/Xserver/xkb/ddxLoad.c,v 3.30 2002/05/31 18:46:06 dawes Exp $ */

#include <stdio.h>
#include <ctype.h>
#define	NEED_EVENTS 1
#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/extensions/XKM.h>
#include "inputstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#define	XKBSRV_NEED_FILE_FUNCS
#include "XKBsrv.h"
#include "XI.h"

#include <errno.h>

#if defined(CSRG_BASED) || defined(linux) || defined(__sgi) || defined(AIXV3) || defined(__osf__) || defined(__GNU__)
#include <paths.h>
#endif

#ifndef PATH_MAX
#ifdef MAXPATHLEN
#define	PATH_MAX MAXPATHLEN
#else
#define	PATH_MAX 1024
#endif
#endif

	/*
	 * If XKM_OUTPUT_DIR specifies a path without a leading slash, it is
	 * relative to the top-level XKB configuration directory.
	 * Making the server write to a subdirectory of that directory
	 * requires some work in the general case (install procedure
	 * has to create links to /var or somesuch on many machines),
	 * so we just compile into /usr/tmp for now.
	 */
#ifndef XKM_OUTPUT_DIR
#define	XKM_OUTPUT_DIR	"compiled/"
#endif

#define	PRE_ERROR_MSG "\"The XKEYBOARD keymap compiler (xkbcomp) reports:\""
#define	ERROR_PREFIX	"\"> \""
#define	POST_ERROR_MSG1 "\"Errors from xkbcomp are not fatal to the X server\""
#define	POST_ERROR_MSG2 "\"End of messages from xkbcomp\""

#ifdef NXAGENT_SERVER

#define NX_XKB_BASE_DIRECTORY            "/usr/lib/X11/xkb"
#define NX_XKB_ALTERNATE_BASE_DIRECTORY  "/usr/share/X11/xkb"
#define NX_KEYMAP_DIR_FILE               "keymap.dir"
#define NX_ALT_XKBCOMP_PATH              "/usr/bin"

static char _NXXkbBasePath[PATH_MAX];
static char _NXXkbCompPath[PATH_MAX];

static int NXVerifyXkbBaseDirectory(const char *dirPath)
{
  int size;
  char *keymapDirFilePath;
  struct stat keymapDirFileStat;

  /*
   * If keymap.dir file
   * is not present into
   * Xkb Base Directory,
   * we suppose that the
   * path is not valid.
   */

  size = strlen(dirPath) + strlen("/") +
             strlen(NX_KEYMAP_DIR_FILE) + 1;

  if ((keymapDirFilePath = malloc((size + 1) * sizeof(char))) == NULL)
  {
    FatalError("NXVerifyXkbBaseDirectory: malloc failed.\n");
  }

  strcpy(keymapDirFilePath, dirPath);
  strcat(keymapDirFilePath, "/");
  strcat(keymapDirFilePath, NX_KEYMAP_DIR_FILE);

  #ifdef TEST
  fprintf(stderr, "NXVerifyXkbBaseDirectory: Looking for [%s] file.\n",
              keymapDirFilePath);
  #endif

  if (stat(keymapDirFilePath, &keymapDirFileStat) != 0)
  {

    #ifdef TEST
    fprintf(stderr, "NXVerifyXkbBaseDirectory: Can't find the keymap.dir file [%s].\n",
                keymapDirFilePath);
    #endif

    free(keymapDirFilePath);

    return 0;
  }

  #ifdef TEST
  fprintf(stderr, "NXVerifyXkbBaseDirectory: Xkb Base Directory [%s] is valid.\n",
              dirPath);
  #endif

  free(keymapDirFilePath);

  return 1;
}

/*
 * This function returns the directory
 * containing the configuration files.
 * This directory is referred by Xkb-
 * BaseDirectory variable (generally
 * it contains the hardcoded path at
 * compile time). If the directory
 * does not exist, the function will
 * try a set of well known directories.
 */

char *_NXGetXkbBasePath(const char *path)
{
  /*
   * Check the xkb base directory only once.
   */

  if (*_NXXkbBasePath != '\0')
  {
    return _NXXkbBasePath;
  }
 
  if (NXVerifyXkbBaseDirectory(XkbBaseDirectory) == 1)
  {
    if (strlen(XkbBaseDirectory) + 1 > PATH_MAX)
    {
      #ifdef TEST
      fprintf(stderr, "_NXGetXkbBasePath: WARNING! Maximum length of xkb base path exceeded.\n");
      #endif
 
      goto _NXGetXkbBasePathError;
    }

    strcpy(_NXXkbBasePath, XkbBaseDirectory);

    #ifdef TEST
    fprintf(stderr, "_NXGetXkbBasePath: Using NX xkb base directory path [%s].\n",
                _NXXkbBasePath);
    #endif

    return _NXXkbBasePath;
  }
 
  if (NXVerifyXkbBaseDirectory(NX_XKB_BASE_DIRECTORY) == 1)
  {
    if (strlen(NX_XKB_BASE_DIRECTORY) + 1 > PATH_MAX)
    {
      #ifdef TEST
      fprintf(stderr, "_NXGetXkbBasePath: WARNING! Maximum length of xkb base path exceeded.\n");
      #endif

      goto _NXGetXkbBasePathError;
    }

    strcpy(_NXXkbBasePath, NX_XKB_BASE_DIRECTORY);

    #ifdef TEST
    fprintf(stderr, "_NXGetXkbBasePath: Using NX xkb base directory path [%s].\n",
                _NXXkbBasePath);
    #endif

    return _NXXkbBasePath;
  }

  if (NXVerifyXkbBaseDirectory(NX_XKB_ALTERNATE_BASE_DIRECTORY) == 1)
  {
    if (strlen(NX_XKB_ALTERNATE_BASE_DIRECTORY) + 1 > PATH_MAX)
    {
      #ifdef TEST
      fprintf(stderr, "_NXGetXkbBasePath: WARNING! Maximum length of xkb base path exceeded.\n");
      #endif

      goto _NXGetXkbBasePathError;
    }

    strcpy(_NXXkbBasePath, NX_XKB_ALTERNATE_BASE_DIRECTORY);

    #ifdef TEST
    fprintf(stderr, "_NXGetXkbBasePath: Using NX xkb base directory path [%s].\n",
                _NXXkbBasePath);
    #endif

    return _NXXkbBasePath;
  }

_NXGetXkbBasePathError:

  if (strlen(path) + 1 > PATH_MAX)
  {
    #ifdef TEST
    fprintf(stderr, "_NXGetXkbBasePath: WARNING! Maximum length of xkb base path exceeded.\n");
    #endif
  }

  strcpy(_NXXkbBasePath, path);

  #ifdef TEST
  fprintf(stderr, "_NXGetXkbBasePath: Using default xkb base path [%s].\n",
              _NXXkbBasePath);
  #endif

  return _NXXkbBasePath;
}

static int NXVerifyXkbCompPath(char *path)
{
  char *xkbCompPath;
  int xkbCompPathSize;
  struct stat xkbCompPathStat;

  if (path == NULL)
  {
    return 0;
  }

  xkbCompPathSize = strlen(path) + strlen("/") +
                        strlen("xkbcomp") + 1;

  if ((xkbCompPath = malloc((xkbCompPathSize + 1) * sizeof(char))) == NULL)
  {
    FatalError("NXVerifyXkbCompPath: WARNING! malloc failed.\n");

    return 0;
  }

  strcpy(xkbCompPath, path);
  strcat(xkbCompPath, "/");
  strcat(xkbCompPath, "xkbcomp");

  if (stat(xkbCompPath, &xkbCompPathStat) != 0)
  {
    #ifdef NX_TRANS_TEST
    fprintf(stderr, "NXVerifyXkbCompPath: WARNING! Failed to stat xkbcomp path [%s].\n",
                xkbCompPath);
    #endif

    free(xkbCompPath);

    return 0;
  }

  free(xkbCompPath);

  return 1;
}

/*
 * This function returns the directory
 * containing the xkbcomp executable.
 * The function will first try to locate
 * the executable in the hardcoded path
 * (the same path as the "base" xkb one)
 * and, if the xkbcomp file couldn't be
 * found, the function will not include
 * an explicit path and will rely on the
 * PATH environment to list the directory.
 */

char *_NXGetXkbCompPath(const char *path)
{

  char * xkbCompPath;

  /*
   * Check the xkbcomp executable
   * directory only once.
   */

  if (*_NXXkbCompPath != '\0')
  {
    return _NXXkbCompPath;
  }

  xkbCompPath = _NXGetXkbBasePath(path);

  if (NXVerifyXkbCompPath(xkbCompPath) == 1)
  {
    if (strlen(xkbCompPath) + 1 > PATH_MAX)
    {
      #ifdef TEST
      fprintf(stderr, "_NXGetXkbCompPath: WARNING! Maximum length of xkbcomp path exceeded.\n");
      #endif

      goto _NXGetXkbCompPathError;
    }

    strcpy(_NXXkbCompPath, xkbCompPath);

    #ifdef TEST
    fprintf(stderr, "_NXGetXkbCompPath: Using xkbcomp path [%s].\n",
                _NXXkbCompPath);
    #endif

    return _NXXkbCompPath;
  }

  xkbCompPath = NX_ALT_XKBCOMP_PATH;

  if (NXVerifyXkbCompPath(xkbCompPath) == 1)
  {
    if (strlen(xkbCompPath) + 1 > PATH_MAX)
    {
      #ifdef TEST
      fprintf(stderr, "_NXGetXkbCompPath: WARNING! Maximum length of xkbcomp path exceeded.\n");
      #endif

      goto _NXGetXkbCompPathError;
    }

    strcpy(_NXXkbCompPath, xkbCompPath);

    #ifdef TEST
    fprintf(stderr, "_NXGetXkbCompPath: Using NX xkbcomp path [%s].\n",
                _NXXkbCompPath);
    #endif

    return _NXXkbCompPath;
  }

_NXGetXkbCompPathError:

  if (strlen(path) + 1 > PATH_MAX)
  {
    #ifdef TEST
    fprintf(stderr, "_NXGetXkbCompPath: WARNING! Maximum length of xkbcomp path exceeded.\n");
    #endif
  }

  strcpy(_NXXkbCompPath, path);

  #ifdef TEST
  fprintf(stderr, "_NXGetXkbCompPath: Using default xkbcomp path [%s].\n",
              _NXXkbCompPath);
  #endif

  return _NXXkbCompPath;
}

#endif

static void
OutputDirectory (outdir)
    char* outdir;
{
#ifndef WIN32
    if (getuid() == 0) {
	/* if server running as root it *may* be able to write */
	/* FIXME: check whether directory is writable at all */
	(void) strcpy (outdir, XKM_OUTPUT_DIR);
    } else
#endif
    {
#ifdef _PATH_VARTMP
	(void) strcpy (outdir, _PATH_VARTMP);
	if (outdir[strlen(outdir) - 1] != '/')	/* Hi IBM, Digital */
	    (void) strcat (outdir, "/");
#else
	(void) strcpy (outdir, "/tmp/");
#endif
    }
}

Bool
#if NeedFunctionPrototypes
XkbDDXCompileNamedKeymap(	XkbDescPtr		xkb,
				XkbComponentNamesPtr	names,
				char *			nameRtrn,
				int			nameRtrnLen)
#else
XkbDDXCompileNamedKeymap(xkb,names,nameRtrn,nameRtrnLen)
    XkbDescPtr			xkb;
    XkbComponentNamesPtr	names;
    char *			nameRtrn;
    int				nameRtrnLen;
#endif
{
char 	cmd[PATH_MAX],file[PATH_MAX],xkm_output_dir[PATH_MAX],*map,*outFile;

    #ifdef TEST
    fprintf(stderr, "XkbDDXCompileNamedKeymap: Got called.\n");
    #endif

    #ifdef __CYGWIN__

    #ifdef TEST
    fprintf(stderr, "XkbDDXCompileNamedKeymap: WARNING! Not implemented on Windows.\n");
    #endif

    return False;

    #endif 

    if (names->keymap==NULL)
	return False;
    strncpy(file,names->keymap,PATH_MAX); file[PATH_MAX-1]= '\0';
    if ((map= strrchr(file,'('))!=NULL) {
	char *tmp;
	if ((tmp= strrchr(map,')'))!=NULL) {
	    *map++= '\0';
	    *tmp= '\0';
	}
	else {
	    map= NULL;
	}
    }
    if ((outFile= strrchr(file,'/'))!=NULL)
	 outFile= _XkbDupString(&outFile[1]);
    else outFile= _XkbDupString(file);
    XkbEnsureSafeMapName(outFile);
    OutputDirectory(xkm_output_dir);

#ifdef NXAGENT_SERVER

    if (_NXGetXkbCompPath(XkbBaseDirectory) != NULL)
    {

#else

    if (XkbBaseDirectory!=NULL) {

#endif

#ifdef __UNIXOS2__

#ifdef NXAGENT_SERVER

        char *tmpbase = (char *) __XOS2RedirRoot(_NXGetXkbCompPath(XkbBaseDirectory));

#else

        char *tmpbase = (char*)__XOS2RedirRoot(XkbBaseDirectory);

#endif

        int i;
	if (strlen(tmpbase)*2+(xkbDebugFlags>9?2:1)
#else

#ifdef NXAGENT_SERVER

        if (strlen(_NXGetXkbCompPath(XkbBaseDirectory)) * 2 +
                (xkbDebugFlags> 9 ? 2 : 1)

#else

	if (strlen(XkbBaseDirectory)*2+(xkbDebugFlags>9?2:1)

#endif

#endif
		+(map?strlen(map)+3:0)+strlen(PRE_ERROR_MSG)
		+strlen(ERROR_PREFIX)+strlen(POST_ERROR_MSG1)
		+strlen(file)+strlen(xkm_output_dir)
		+strlen(outFile)+53 > PATH_MAX)
	{
	    ErrorF("compiler command for keymap (%s) exceeds max length\n",
								names->keymap);
	    return False;
	}
#ifndef __UNIXOS2__

#ifdef NXAGENT_SERVER

        sprintf(cmd, "%s/xkbcomp -w %d -R%s -xkm %s%s -em1 %s -emp %s -eml %s keymap/%s %s%s.xkm",
                    _NXGetXkbCompPath(XkbBaseDirectory), ((xkbDebugFlags < 2) ? 1 :
                        ((xkbDebugFlags > 10) ? 10 : (int) xkbDebugFlags)),
                            XkbBaseDirectory, (map ? "-m " : ""),
                                (map ? map : ""), PRE_ERROR_MSG, ERROR_PREFIX,
                                    POST_ERROR_MSG1, file, xkm_output_dir, outFile);

#else

	sprintf(cmd,"%s/xkbcomp -w %d -R%s -xkm %s%s -em1 %s -emp %s -eml %s keymap/%s %s%s.xkm",
		XkbBaseDirectory,
		((xkbDebugFlags<2)?1:((xkbDebugFlags>10)?10:(int)xkbDebugFlags)),
		XkbBaseDirectory,(map?"-m ":""),(map?map:""),
		PRE_ERROR_MSG,ERROR_PREFIX,POST_ERROR_MSG1,file,
		xkm_output_dir,outFile);

#endif

#else
	for (i=0; i<strlen(tmpbase); i++) if (tmpbase[i]=='/') tmpbase[i]='\\';
	sprintf(cmd,"%s\\xkbcomp -w %d -R%s -xkm %s%s -em1 %s -emp %s -eml %s keymap/%s %s%s.xkm",
		tmpbase,
		((xkbDebugFlags<2)?1:((xkbDebugFlags>10)?10:(int)xkbDebugFlags)),
		tmpbase,(map?"-m ":""),(map?map:""),
		PRE_ERROR_MSG,ERROR_PREFIX,POST_ERROR_MSG1,file,
		xkm_output_dir,outFile);
	ErrorF("Command line for XKB is %s\n",cmd);
#endif
    }
    else {
	if ((xkbDebugFlags>9?2:1)+(map?strlen(map)+3:0)+strlen(PRE_ERROR_MSG)
		+strlen(ERROR_PREFIX)+strlen(POST_ERROR_MSG1)
		+strlen(file)+strlen(xkm_output_dir)
		+strlen(outFile)+49 > PATH_MAX)
	{
            ErrorF("compiler command for keymap (%s) exceeds max length\n",
							names->keymap);
	    return False;
	}
	sprintf(cmd,"xkbcomp -w %d -xkm %s%s -em1 %s -emp %s -eml %s keymap/%s %s%s.xkm",
		((xkbDebugFlags<2)?1:((xkbDebugFlags>10)?10:(int)xkbDebugFlags)),
		(map?"-m ":""),(map?map:""),
		PRE_ERROR_MSG,ERROR_PREFIX,POST_ERROR_MSG1,file,
		xkm_output_dir,outFile);
    }
#ifdef DEBUG
    if (xkbDebugFlags) {
	ErrorF("XkbDDXCompileNamedKeymap compiling keymap using:\n");
	ErrorF("    \"cmd\"\n");
    }
#endif
    if (System(cmd)==0) {
	if (nameRtrn) {
	    strncpy(nameRtrn,outFile,nameRtrnLen);
	    nameRtrn[nameRtrnLen-1]= '\0';
	}
	if (outFile!=NULL)
	    _XkbFree(outFile);
	return True;
    } 
#ifdef DEBUG
    ErrorF("Error compiling keymap (%s)\n",names->keymap);
#endif
    if (outFile!=NULL)
	_XkbFree(outFile);
    return False;
}
        	
Bool    	
#if NeedFunctionPrototypes
XkbDDXCompileKeymapByNames(	XkbDescPtr		xkb,
				XkbComponentNamesPtr	names,
				unsigned		want,
				unsigned		need,
				char *			nameRtrn,
				int			nameRtrnLen)
#else
XkbDDXCompileKeymapByNames(xkb,names,want,need,nameRtrn,nameRtrnLen)
    XkbDescPtr			xkb;
    XkbComponentNamesPtr	names;
    unsigned			want;
    unsigned			need;
    char *			nameRtrn;
    int				nameRtrnLen;
#endif
{
FILE *	out;
char	buf[PATH_MAX],keymap[PATH_MAX],xkm_output_dir[PATH_MAX];
#ifdef WIN32
char tmpname[32];
#endif    
#ifdef __UNIXOS2__
char *tmpbase;
int i;
#endif

    /*
     * This function gets called by ProcXkbGetKbdByName(), that
     * in turn is called by the dispatcher. It seems to be used
     * by OpenOffice to read some keyboard configuration any
     * time a drop down menu is opened. It seems to always fail
     * with the following error:
     *
     * ---
     * The XKEYBOARD keymap compiler (xkbcomp) reports:
     * > Error:            Can't find file "unknown" for geometry include
     * >                   Exiting
     * >                   Abandoning geometry file "default"
     * ---
     */

    #ifdef TEST
    fprintf(stderr, "XkbDDXCompileKeymapByNames: Got called with keymap [%s].\n",
                names->keymap);
    #endif

    #ifdef __CYGWIN__

    #ifdef TEST
    fprintf(stderr, "XkbDDXCompileKeymapByNames: WARNING! Not implemented on Windows.\n");
    #endif

    return False;

    #endif 

    if ((names->keymap==NULL)||(names->keymap[0]=='\0')) {
	sprintf(keymap,"server-%s",display);
    }
    else {
	if (strlen(names->keymap) > PATH_MAX - 1) {
	    ErrorF("name of keymap (%s) exceeds max length\n", names->keymap);
	    return False;
	}
	strcpy(keymap,names->keymap);
    }

    XkbEnsureSafeMapName(keymap);
    OutputDirectory(xkm_output_dir);
#ifdef WIN32
    strcpy(tmpname, "\\temp\\xkb_XXXXXX");
    (void) mktemp(tmpname);
#endif
#ifdef __UNIXOS2__

#ifdef NXAGENT_SERVER

    tmpbase = (char *) __XOS2RedirRoot(_NXGetXkbCompPath(XkbBaseDirectory));

#else

    tmpbase = (char*)__XOS2RedirRoot(XkbBaseDirectory);

#endif

#endif

#ifdef NXAGENT_SERVER

    if (_NXGetXkbCompPath(XkbBaseDirectory) != NULL)
    {
      if (strlen(_NXGetXkbCompPath(XkbBaseDirectory)) * 2 +
              (xkbDebugFlags > 9 ? 2 : 1) +
                  strlen(PRE_ERROR_MSG) + strlen(ERROR_PREFIX) +
                      strlen(POST_ERROR_MSG1) + strlen(xkm_output_dir) +
                          strlen(keymap) + 48 > PATH_MAX)

#else

    if (XkbBaseDirectory!=NULL) {
	if (strlen(XkbBaseDirectory)*2+(xkbDebugFlags>9?2:1)
		+strlen(PRE_ERROR_MSG)+strlen(ERROR_PREFIX)
		+strlen(POST_ERROR_MSG1)+strlen(xkm_output_dir)
		+strlen(keymap)+48 > PATH_MAX)

#endif

	{
            ErrorF("compiler command for keymap (%s) exceeds max length\n",
							names->keymap);
	    return False;
	}
#ifndef WIN32
#ifndef __UNIXOS2__

#ifdef NXAGENT_SERVER

        sprintf(buf, "%s/xkbcomp -w %d -R%s -xkm - -em1 %s -emp %s -eml %s \"%s%s.xkm\"",
                    _NXGetXkbCompPath(XkbBaseDirectory), ((xkbDebugFlags < 2) ? 1 :
                        ((xkbDebugFlags > 10) ? 10 : (int) xkbDebugFlags)),
                            XkbBaseDirectory, PRE_ERROR_MSG, ERROR_PREFIX,
                                POST_ERROR_MSG1, xkm_output_dir, keymap);

#else

	sprintf(buf,
	   "%s/xkbcomp -w %d -R%s -xkm - -em1 %s -emp %s -eml %s \"%s%s.xkm\"",
		XkbBaseDirectory,
		((xkbDebugFlags<2)?1:((xkbDebugFlags>10)?10:(int)xkbDebugFlags)),
		XkbBaseDirectory,
		PRE_ERROR_MSG,ERROR_PREFIX,POST_ERROR_MSG1,
		xkm_output_dir,keymap);

#endif

#else
	for (i=0; i<strlen(tmpbase); i++) if (tmpbase[i]=='/') tmpbase[i]='\\';
	sprintf(buf,
	  "%s\\xkbcomp -w %d -R%s -xkm - -em1 %s -emp %s -eml %s \"%s%s.xkm\"",
		tmpbase,
		((xkbDebugFlags<2)?1:((xkbDebugFlags>10)?10:(int)xkbDebugFlags)),
		tmpbase,
		PRE_ERROR_MSG,ERROR_PREFIX,POST_ERROR_MSG1,
		xkm_output_dir,keymap);
#endif
#else

#ifdef NXAGENT_SERVER

        sprintf(buf, "%s/xkbcomp -w %d -R%s -xkm - -em1 %s -emp %s -eml %s \"%s%s.xkm\" < %s",
                    _NXGetXkbCompPath(XkbBaseDirectory), ((xkbDebugFlags < 2 ) ? 1 :
                        ((xkbDebugFlags > 10) ? 10 : (int) xkbDebugFlags)),
                            XkbBaseDirectory,  PRE_ERROR_MSG, ERROR_PREFIX,
                                POST_ERROR_MSG1, xkm_output_dir, keymap, tmpname);

#else

	sprintf(buf,
      "%s/xkbcomp -w %d -R%s -xkm - -em1 %s -emp %s -eml %s \"%s%s.xkm\" < %s",
		XkbBaseDirectory,
		((xkbDebugFlags<2)?1:((xkbDebugFlags>10)?10:(int)xkbDebugFlags)),
		XkbBaseDirectory,
		PRE_ERROR_MSG,ERROR_PREFIX,POST_ERROR_MSG1,
		xkm_output_dir,keymap,tmpname);

#endif

#endif
    }
    else {
	if ((xkbDebugFlags>9?2:1)+strlen(PRE_ERROR_MSG)
		+strlen(ERROR_PREFIX)+strlen(POST_ERROR_MSG1)
		+strlen(xkm_output_dir)+strlen(keymap)+44 > PATH_MAX)
	{
            ErrorF("compiler command for keymap (%s) exceeds max length\n",
							names->keymap);
	    return False;
	}
#ifndef WIN32
	sprintf(buf,
		"xkbcomp -w %d -xkm - -em1 %s -emp %s -eml %s \"%s%s.xkm\"",
		((xkbDebugFlags<2)?1:((xkbDebugFlags>10)?10:(int)xkbDebugFlags)),
		PRE_ERROR_MSG,ERROR_PREFIX,POST_ERROR_MSG1,
		xkm_output_dir,keymap);
#else
	sprintf(buf,
	      "xkbcomp -w %d -xkm - -em1 %s -emp %s -eml %s \"%s%s.xkm\" < %s",
		((xkbDebugFlags<2)?1:((xkbDebugFlags>10)?10:(int)xkbDebugFlags)),
		PRE_ERROR_MSG,ERROR_PREFIX,POST_ERROR_MSG1,
		xkm_output_dir,keymap,tmpname);
#endif
    }

    #ifdef TEST
    fprintf(stderr, "XkbDDXCompileKeymapByNames: Executing command [%s].\n", buf);
    #endif

#ifndef WIN32
    out= Popen(buf,"w");
#else
    out= fopen(tmpname, "w");
#endif
    if (out!=NULL) {
#ifdef DEBUG
    if (xkbDebugFlags) {
       ErrorF("XkbDDXCompileKeymapByNames compiling keymap:\n");
       XkbWriteXKBKeymapForNames(stderr,names,NULL,xkb,want,need);
    }
#endif
	XkbWriteXKBKeymapForNames(out,names,NULL,xkb,want,need);
#ifndef WIN32
#ifdef __sun
        if (Pclose(out) != 0)
        {
            ErrorF("Warning: Spurious failure reported in Pclose() runnning 'xkbcomp'.\n");
        }
        if (1)
#else
	if (Pclose(out)==0)
#endif
#else
	if (fclose(out)==0)
#endif
	{
#ifdef WIN32
	    if (System(buf) < 0)
		ErrorF("Could not invoke keymap compiler\n");
	    else {
#endif
	    if (nameRtrn) {
		strncpy(nameRtrn,keymap,nameRtrnLen);
		nameRtrn[nameRtrnLen-1]= '\0';
	    }
#if defined(Lynx) && defined(__i386__) && defined(NEED_POPEN_WORKAROUND)
	/* somehow popen/pclose is broken on LynxOS AT 2.3.0/2.4.0!
	 * the problem usually shows up with XF86Setup
	 * this hack waits at max 5 seconds after pclose() returns
	 * for the output of the xkbcomp output file.
	 * I didn't manage to get a patch in time for the 3.2 release
	 */
            {
		int i;
		char name[PATH_MAX];

#ifdef NXAGENT_SERVER

                if (_NXGetXkbCompPath(XkbBaseDirectory) != NULL)
                {
                  sprintf(name, "%s/%s%s.xkm", _NXGetXkbCompPath(XkbBaseDirectory),
                              xkm_output_dir, keymap);
                }

#else

                if (XkbBaseDirectory!=NULL)
		    sprintf(name,"%s/%s%s.xkm", XkbBaseDirectory
			,xkm_output_dir, keymap);

#endif

		else
                    sprintf(name,"%s%s.xkm", xkm_output_dir, keymap);
		for (i = 0; i < 10; i++) {
	            if (access(name, 0) == 0) break;
		    usleep(500000);
		}
#ifdef DEBUG
		if (i) ErrorF(">>>> Waited %d times for %s\n", i, name);
#endif
	    }
#endif
	    return True;
#ifdef WIN32
	    }
#endif
	}
#ifdef DEBUG
	else
	    ErrorF("Error compiling keymap (%s)\n",keymap);
#endif
    }
#ifdef DEBUG
    else {
#ifndef WIN32
	ErrorF("Could not invoke keymap compiler\n");
#else
	ErrorF("Could not open file %s\n", tmpname);
#endif
    }
#endif
    if (nameRtrn)
	nameRtrn[0]= '\0';
    return False;
}

FILE *
#if NeedFunctionPrototypes
XkbDDXOpenConfigFile(char *mapName,char *fileNameRtrn,int fileNameRtrnLen)
#else
XkbDDXOpenConfigFile(mapName,fileNameRtrn,fileNameRtrnLen)
    char *	mapName;
    char *	fileNameRtrn;
    int		fileNameRtrnLen;
#endif
{
char	buf[PATH_MAX],xkm_output_dir[PATH_MAX];
FILE *	file;

    buf[0]= '\0';
    if (mapName!=NULL) {
	OutputDirectory(xkm_output_dir);
	if ((XkbBaseDirectory!=NULL)&&(xkm_output_dir[0]!='/')) {
	    if (strlen(XkbBaseDirectory)+strlen(xkm_output_dir)
		     +strlen(mapName)+6 <= PATH_MAX)
	    {
	        sprintf(buf,"%s/%s%s.xkm",XkbBaseDirectory,
					xkm_output_dir,mapName);
	    }
	}
	else if (strlen(xkm_output_dir)+strlen(mapName)+5 <= PATH_MAX)
	    sprintf(buf,"%s%s.xkm",xkm_output_dir,mapName);
	if (buf[0] != '\0')
	    file= fopen(buf,"r");
	else file= NULL;
    }
    else file= NULL;
    if ((fileNameRtrn!=NULL)&&(fileNameRtrnLen>0)) {
	strncpy(fileNameRtrn,buf,fileNameRtrnLen);
	buf[fileNameRtrnLen-1]= '\0';
    }
    return file;
}

unsigned
#if NeedFunctionPrototypes
XkbDDXLoadKeymapByNames(	DeviceIntPtr		keybd,
				XkbComponentNamesPtr	names,
				unsigned		want,
				unsigned		need,
				XkbFileInfo *		finfoRtrn,
				char *			nameRtrn,
				int 			nameRtrnLen)
#else
XkbDDXLoadKeymapByNames(keybd,names,want,need,finfoRtrn,nameRtrn,nameRtrnLen)
    DeviceIntPtr		keybd;
    XkbComponentNamesPtr	names;
    unsigned			want;
    unsigned			need;
    XkbFileInfo *		finfoRtrn;
    char *			nameRtrn;
    int 			nameRtrnLen;
#endif
{
XkbDescPtr	xkb;
FILE	*	file;
char		fileName[PATH_MAX];
unsigned	missing;

    bzero(finfoRtrn,sizeof(XkbFileInfo));
    if ((keybd==NULL)||(keybd->key==NULL)||(keybd->key->xkbInfo==NULL))
	 xkb= NULL;
    else xkb= keybd->key->xkbInfo->desc;
    if ((names->keycodes==NULL)&&(names->types==NULL)&&
	(names->compat==NULL)&&(names->symbols==NULL)&&
	(names->geometry==NULL)) {
	if (names->keymap==NULL) {
	    bzero(finfoRtrn,sizeof(XkbFileInfo));
	    if (xkb && XkbDetermineFileType(finfoRtrn,XkbXKMFile,NULL) &&
	   				((finfoRtrn->defined&need)==need) ) {
		finfoRtrn->xkb= xkb;
		nameRtrn[0]= '\0';
		return finfoRtrn->defined;
	    }
	    return 0;
	}
	else if (!XkbDDXCompileNamedKeymap(xkb,names,nameRtrn,nameRtrnLen)) {
#ifdef NOISY
	    ErrorF("Couldn't compile keymap file\n");
#endif
	    return 0;
	}
    }
    else if (!XkbDDXCompileKeymapByNames(xkb,names,want,need,
						nameRtrn,nameRtrnLen)){
#ifdef NOISY
	ErrorF("Couldn't compile keymap file\n");
#endif
	return 0;
    }
    file= XkbDDXOpenConfigFile(nameRtrn,fileName,PATH_MAX);
    if (file==NULL) {
	ErrorF("Couldn't open compiled keymap file %s\n",fileName);
	return 0;
    }
    missing= XkmReadFile(file,need,want,finfoRtrn);
    if (finfoRtrn->xkb==NULL) {
	ErrorF("Error loading keymap %s\n",fileName);
	fclose(file);
	(void) unlink (fileName);
	return 0;
    }
#ifdef DEBUG
    else if (xkbDebugFlags) {
	ErrorF("Loaded %s, defined=0x%x\n",fileName,finfoRtrn->defined);
    }
#endif
    fclose(file);
    (void) unlink (fileName);
    return (need|want)&(~missing);
}

Bool
#if NeedFunctionPrototypes
XkbDDXNamesFromRules(	DeviceIntPtr		keybd,
			char *			rules_name,
			XkbRF_VarDefsPtr	defs,
			XkbComponentNamesPtr	names)
#else
XkbDDXNamesFromRules(keybd,rules_name,defs,names)
    DeviceIntPtr		keybd;
    char *			rules_name;
    XkbRF_VarDefsPtr		defs;
    XkbComponentNamesPtr	names;
#endif
{
char 		buf[PATH_MAX];
FILE *		file;
Bool		complete;
XkbRF_RulesPtr	rules;

    if (!rules_name)
	return False;
    if (XkbBaseDirectory==NULL) {
	if (strlen(rules_name)+7 > PATH_MAX)
	    return False;
	sprintf(buf,"rules/%s",rules_name);
    }
    else {
	if (strlen(XkbBaseDirectory)+strlen(rules_name)+8 > PATH_MAX)
	    return False;
        sprintf(buf,"%s/rules/%s",XkbBaseDirectory,rules_name);
    }
    if ((file= fopen(buf,"r"))==NULL)
	return False;
    if ((rules= XkbRF_Create(0,0))==NULL) {
	fclose(file);
	return False;
    }
    if (!XkbRF_LoadRules(file,rules)) {
	fclose(file);
	XkbRF_Free(rules,True);
	return False;
    }
    bzero((char *)names,sizeof(XkbComponentNamesRec));
    complete= XkbRF_GetComponents(rules,defs,names);
    fclose(file);
    XkbRF_Free(rules,True);
    return complete;
}


