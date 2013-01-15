/*
 *Copyright (C) 1994-2000 The XFree86 Project, Inc. All Rights Reserved.
 *
 *Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 *"Software"), to deal in the Software without restriction, including
 *without limitation the rights to use, copy, modify, merge, publish,
 *distribute, sublicense, and/or sell copies of the Software, and to
 *permit persons to whom the Software is furnished to do so, subject to
 *the following conditions:
 *
 *The above copyright notice and this permission notice shall be
 *included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE XFREE86 PROJECT BE LIABLE FOR
 *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 *CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *Except as contained in this notice, the name of the XFree86 Project
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from the XFree86 Project.
 *
 * Authors:	Dakshinamurthy Karra
 *		Suhaib M Siddiqi
 *		Peter Busch
 *		Harold L Hunt II
 *		Kensuke Matsuzaki
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winscrinit.c,v 1.26 2003/02/12 15:01:38 alanh Exp $ */

/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXWIN, NX protocol compression and NX extensions to this software      */
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

#define WINVER 0x0501

#include "win.h"

#ifdef NXWIN_LOGO
  int nxagent_depth;
  int nxagent_white, nxagent_red, nxagent_black;
  unsigned int r, b, g, or, ob, og, off;
#endif

#ifdef NXWIN_PONG
  UINT valPingPong = 0;
#endif

#if defined(NXWIN_KILL) || defined(NXWIN_EXIT)
  UINT valNxKill = 0;
  UINT valConnected = 0;
  UINT valNxMessage = 0;
  UINT valKillESD   = 0;
  DWORD dwProcessId[10]= {0};
  UINT storedProxyPid =0;
  UINT isProxyRunning =0;
  UINT setDisplay     =0; 
  unsigned int currentProxyPid=-1;
  UINT stored_nxserver_version = 0; 
  BOOL isToShowMessageBox = FALSE;
  UINT valNxAdminCommand  = 0; 
#endif

#ifdef NXWIN_LOGO
unsigned int nxLogoColor(unsigned int colorin) {
  int cr=0, cg=0, cb=0;

  cr = ((colorin >> or) & r);
  cg = ((colorin >> (og-8)) & g);
  cb = ((colorin >> (ob-16)) & b);

  return cr| cg| cb;
}
#endif

#ifndef NXWIN_OLD__CURSORS

extern int nxwinOldCursors;
extern int nxwinAlphaCaps;
extern OSVERSIONINFO nxwinOsVersionInfo;

extern miPointerSpriteFuncRec nxwinPointerSpriteFuncs;
extern int nxwinCursorWidth;
extern int nxwinCursorHeight;



#endif

/*
 * Determine what type of screen we are initializing
 * and call the appropriate procedure to intiailize
 * that type of screen.
 */

Bool
winScreenInit (int index,
	       ScreenPtr pScreen,
	       int argc, char **argv)
{
  winScreenInfoPtr      pScreenInfo = &g_ScreenInfo[index];
  winPrivScreenPtr	pScreenPriv;
  HDC			hdc;

#if CYGDEBUG
  ErrorF ("winScreenInit - dwWidth: %d dwHeight: %d\n",
	  pScreenInfo->dwWidth, pScreenInfo->dwHeight);
#endif

  /* Allocate privates for this screen */
  if (!winAllocatePrivates (pScreen))
    {
      ErrorF ("winScreenInit - Couldn't allocate screen privates\n");
      return FALSE;
    }

  /* Get a pointer to the privates structure that was allocated */
  pScreenPriv = winGetScreenPriv (pScreen);

  /* Save a pointer to this screen in the screen info structure */
  pScreenInfo->pScreen = pScreen;

  /* Save a pointer to the screen info in the screen privates structure */
  /* This allows us to get back to the screen info from a screen pointer */
  pScreenPriv->pScreenInfo = pScreenInfo;

  /*
   * Determine which engine to use.
   *
   * NOTE: This is done once per screen because each screen possibly has
   * a preferred engine specified on the command line.
   */
  if (!winSetEngine (pScreen))
    {
      ErrorF ("winScreenInit - winSetEngine () failed\n");
      return FALSE;
    }

  /* Adjust the video mode for our engine type */
  if (!(*pScreenPriv->pwinAdjustVideoMode) (pScreen))
    {
      ErrorF ("winScreenInit - winAdjustVideoMode () failed\n");
      return FALSE;
    }

  /* Check for supported display depth */
  if (!(WIN_SUPPORTED_BPPS & (1 << (pScreenInfo->dwBPP - 1))))
    {
      ErrorF ("winScreenInit - Unsupported display depth: %d\n" \
	      "Change your Windows display depth to 15, 16, 24, or 32 bits "
	      "per pixel.\n",
	      pScreenInfo->dwBPP);
      ErrorF ("winScreenInit - Supported depths: %08x\n",
	      WIN_SUPPORTED_BPPS);
#if WIN_CHECK_DEPTH
      return FALSE;
#endif
    }

  /*
   * Check that all monitors have the same display depth if we are using
   * multiple monitors
   */
  if (pScreenInfo->fMultipleMonitors 
      && !GetSystemMetrics (SM_SAMEDISPLAYFORMAT))
    {
      ErrorF ("winScreenInit - Monitors do not all have same pixel format / "
	      "display depth.\n"
	      "Using primary display only.\n");
      pScreenInfo->fMultipleMonitors = FALSE;
    }

  /* Get a device context */
  hdc = GetDC (pScreenPriv->hwndScreen);

  /* Store the initial height, width, and depth of the display */
  /* Are we using multiple monitors? */
  if (pScreenInfo->fMultipleMonitors)
    {
      pScreenPriv->dwLastWindowsWidth = GetSystemMetrics (SM_CXVIRTUALSCREEN);
      pScreenPriv->dwLastWindowsHeight = GetSystemMetrics (SM_CYVIRTUALSCREEN);

      /* 
       * In this case, some of the defaults set in
       * winInitializeDefaultScreens () are not correct ...
       */
      if (!pScreenInfo->fUserGaveHeightAndWidth)
	{
	  pScreenInfo->dwInitialX = GetSystemMetrics (SM_XVIRTUALSCREEN);
	  pScreenInfo->dwInitialY = GetSystemMetrics (SM_YVIRTUALSCREEN);
	  pScreenInfo->dwWidth = GetSystemMetrics (SM_CXVIRTUALSCREEN);
	  pScreenInfo->dwHeight = GetSystemMetrics (SM_CYVIRTUALSCREEN);
	  pScreenInfo->dwWidth_mm = (pScreenInfo->dwWidth /
				     WIN_DEFAULT_DPI) * 25.4;
	  pScreenInfo->dwHeight_mm = (pScreenInfo->dwHeight /
				      WIN_DEFAULT_DPI) * 25.4;
	}
    }
  else
    {
      pScreenPriv->dwLastWindowsWidth = GetSystemMetrics (SM_CXSCREEN);
      pScreenPriv->dwLastWindowsHeight = GetSystemMetrics (SM_CYSCREEN);
    }

  /*
   * Get current screen info.
   */

  if (!pScreenInfo->fMultipleMonitors)
  {
    POINT point;
    HMONITOR hMonitor;
    MONITORINFO mi;

    if (GetCursorPos(&point) != 0)
    {
      hMonitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);

      if (hMonitor != NULL)
      {
        ZeroMemory(&mi, sizeof(MONITORINFO));

        mi.cbSize = sizeof(MONITORINFO);

        if (GetMonitorInfo(hMonitor, &mi) != 0)
        {
          pScreenInfo -> dwInitialX = mi.rcMonitor.left;
          pScreenInfo -> dwInitialY = mi.rcMonitor.top;

          pScreenInfo->dwWidth = mi.rcMonitor.right - mi.rcMonitor.left;
          pScreenInfo->dwHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
          pScreenInfo->dwWidth_mm = (pScreenInfo->dwWidth /
                                        WIN_DEFAULT_DPI) * 25.4;
          pScreenInfo->dwHeight_mm = (pScreenInfo->dwHeight /
                                         WIN_DEFAULT_DPI) * 25.4;

          pScreenInfo->hMonitor = hMonitor;
        }
      }
    }
  }

  /* Create display window */
  if (!(*pScreenPriv->pwinCreateBoundingWindow) (pScreen))
    {
      ErrorF ("winScreenInit - pwinCreateBoundingWindow () "
	      "failed\n");
      return FALSE;
    }

  fprintf(stderr, "Info: Display running with pid '%lu' and handler '0x%lx'.\n",
              (long unsigned) getpid(), (long unsigned) pScreenPriv ->
                  hwndScreen);

  /* Save the original bits per pixel */
  pScreenPriv->dwLastWindowsBitsPixel = GetDeviceCaps (hdc, BITSPIXEL);

  #ifndef NXWIN_OLD_CURSORS

  #define SHADEBLENDCAPS 120

  nxwinAlphaCaps = GetDeviceCaps(hdc, SHADEBLENDCAPS);

  ErrorF ("winFinishScreenInitFB - Device blending capability [%d].\n", r);

  nxwinOsVersionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

  GetVersionEx (&nxwinOsVersionInfo);

  #endif

  /* Release the device context */
  ReleaseDC (pScreenPriv->hwndScreen, hdc);
    
  /* Clear the visuals list */
  miClearVisualTypes ();
  
  /* Set the padded screen width */
  pScreenInfo->dwPaddedWidth = PixmapBytePad (pScreenInfo->dwWidth,
					      pScreenInfo->dwBPP);

  /* Call the engine dependent screen initialization procedure */
  if (!((*pScreenPriv->pwinFinishScreenInit) (index, pScreen, argc, argv)))
    {
      ErrorF ("winScreenInit - winFinishScreenInit () failed\n");
      return FALSE;
    }

#if CYGDEBUG || YES
  ErrorF ("winScreenInit - returning\n");
#endif

  return TRUE;
}


/* See Porting Layer Definition - p. 20 */
Bool
winFinishScreenInitFB (int index,
		       ScreenPtr pScreen,
		       int argc, char **argv)
{
  winScreenPriv(pScreen);
  winScreenInfo		*pScreenInfo = pScreenPriv->pScreenInfo;
  VisualPtr		pVisual = NULL;
  char			*pbits = NULL;
  int			iReturn;

#if WIN_LAYER_SUPPORT
  pScreenPriv->dwLayerKind = LAYER_SHADOW;
#endif

  /* Create framebuffer */
  if (!(*pScreenPriv->pwinAllocateFB) (pScreen))
    {
      ErrorF ("winFinishScreenInitFB - Could not allocate framebuffer\n");
      return FALSE;
    }

  /*
   * Grab the number of bits that are used to represent color in each pixel.
   */
  if (pScreenInfo->dwBPP == 8)
    pScreenInfo->dwDepth = 8;
  else
    pScreenInfo->dwDepth = winCountBits (pScreenPriv->dwRedMask)
      + winCountBits (pScreenPriv->dwGreenMask)
      + winCountBits (pScreenPriv->dwBlueMask);
  
  ErrorF ("winFinishScreenInitFB - Masks: %08x %08x %08x\n",
	  pScreenPriv->dwRedMask, pScreenPriv->dwGreenMask,
	  pScreenPriv->dwBlueMask);

  /* Init visuals */
  if (!(*pScreenPriv->pwinInitVisuals) (pScreen))
    {
      ErrorF ("winFinishScreenInitFB - winInitVisuals failed\n");
      return FALSE;
    }

  /* Setup a local variable to point to the framebuffer */
  pbits = pScreenInfo->pfb;
  
  /* Apparently we need this for the render extension */
  miSetPixmapDepths ();

  /* Start fb initialization */
  if (!fbSetupScreen (pScreen,
		      pScreenInfo->pfb,
		      pScreenInfo->dwWidth, pScreenInfo->dwHeight,
		      monitorResolution, monitorResolution,
		      pScreenInfo->dwStride,
		      pScreenInfo->dwBPP))
    {
      ErrorF ("winFinishScreenInitFB - fbSetupScreen failed\n");
      return FALSE;
    }

  /* Override default colormap routines if visual class is dynamic */
  if (pScreenInfo->dwDepth == 8
      && (pScreenInfo->dwEngine == WIN_SERVER_SHADOW_GDI
	  || (pScreenInfo->dwEngine == WIN_SERVER_SHADOW_DDNL
	      && pScreenInfo->fFullScreen)
	  || (pScreenInfo->dwEngine == WIN_SERVER_SHADOW_DD
	      && pScreenInfo->fFullScreen)))
    {
      pScreen->CreateColormap = winCreateColormap;
      pScreen->DestroyColormap = winDestroyColormap;
      pScreen->InstallColormap = winInstallColormap;
      pScreen->UninstallColormap = winUninstallColormap;
      pScreen->ListInstalledColormaps = winListInstalledColormaps;
      pScreen->StoreColors = winStoreColors;
      pScreen->ResolveColor = winResolveColor;

      /*
       * NOTE: Setting whitePixel to 255 causes Magic 7.1 to allocate its
       * own colormap, as it cannot allocate 7 planes in the default
       * colormap.  Setting whitePixel to 1 allows Magic to get 7
       * planes in the default colormap, so it doesn't create its
       * own colormap.  This latter situation is highly desireable,
       * as it keeps the Magic window viewable when switching to
       * other X clients that use the default colormap.
       */
      pScreen->blackPixel = 0;
      pScreen->whitePixel = 1;
    }

  /* Place our save screen function */
  pScreen->SaveScreen = winSaveScreen;

  /* Backing store functions */
  /*
   * FIXME: Backing store support still doesn't seem to be working.
   */
  pScreen->BackingStoreFuncs.SaveAreas = fbSaveAreas;
  pScreen->BackingStoreFuncs.RestoreAreas = fbRestoreAreas;

  /* Finish fb initialization */
  if (!fbFinishScreenInit (pScreen,
			   pScreenInfo->pfb,
			   pScreenInfo->dwWidth, pScreenInfo->dwHeight,
			   monitorResolution, monitorResolution,
			   pScreenInfo->dwStride,
			   pScreenInfo->dwBPP))
    {
      ErrorF ("winFinishScreenInitFB - fbFinishScreenInit failed\n");
      return FALSE;
    }

  /* Save a pointer to the root visual */
  for (pVisual = pScreen->visuals;
       pVisual->vid != pScreen->rootVisual;
       pVisual++);
  pScreenPriv->pRootVisual = pVisual;

  /* 
   * Setup points to the block and wakeup handlers.  Pass a pointer
   * to the current screen as pWakeupdata.
   */
  pScreen->BlockHandler = winBlockHandler;
  pScreen->WakeupHandler = winWakeupHandler;
  pScreen->blockData = pScreen;
  pScreen->wakeupData = pScreen;

#ifdef RENDER
  /* Render extension initialization, calls miPictureInit */
  if (!fbPictureInit (pScreen, NULL, 0))
    {
      ErrorF ("winFinishScreenInitFB - fbPictureInit () failed\n");
      return FALSE;
    }
#endif

#if WIN_LAYER_SUPPORT
  /* KDrive does LayerStartInit right after fbPictureInit */
  if (!LayerStartInit (pScreen))
    {
      ErrorF ("winFinishScreenInitFB - LayerStartInit () failed\n");
      return FALSE;
    }

  /* Not sure what we're adding to shadow, but add it anyway */
  if (!shadowAdd (pScreen, 0, pScreenPriv->pwinShadowUpdate, NULL, 0, 0))
    {
      ErrorF ("winFinishScreenInitFB - shadowAdd () failed\n");
      return FALSE;
    }

  /* KDrive does LayerFinishInit right after LayerStartInit */
  if (!LayerFinishInit (pScreen))
    {
      ErrorF ("winFinishScreenInitFB - LayerFinishInit () failed\n");
      return FALSE;
    }

  /* KDrive does LayerCreate right after LayerFinishInit */
  pScreenPriv->pLayer = winLayerCreate (pScreen);
  if (!pScreenPriv->pLayer)
    {
      ErrorF ("winFinishScreenInitFB - winLayerCreate () failed\n");
      return FALSE;
    }
  
  /* KDrive does RandRInit right after LayerCreate */
#ifdef RANDR
  if (pScreenInfo->dwDepth != 8 && !winRandRInit (pScreen))
    {
      ErrorF ("winFinishScreenInitFB - winRandRInit () failed\n");
      return FALSE;
    }
#endif
#endif

  /*
   * Backing store support should reduce network traffic and increase
   * performance.
   */
  miInitializeBackingStore (pScreen);

  /* KDrive does miDCInitialize right after miInitializeBackingStore */
  /* Setup the cursor routines */
#if CYGDEBUG
  ErrorF ("winFinishScreenInitFB - Calling miDCInitialize ()\n");
#endif

#ifdef NXWIN_OLD_CURSORS

  miDCInitialize (pScreen, &g_winPointerCursorFuncs);

#else

  if (nxwinOldCursors == 1)
  {
    miDCInitialize (pScreen, &g_winPointerCursorFuncs);
  }
  else
  {
    miPointerInitialize (pScreen, &nxwinPointerSpriteFuncs,
                               &g_winPointerCursorFuncs, 1);

    nxwinCursorWidth = GetSystemMetrics(SM_CXCURSOR);
    nxwinCursorHeight = GetSystemMetrics(SM_CYCURSOR);

    #ifdef DEBUG
    ErrorF ("winFinishScreenInitFB - System cursor size [%d, %d].\n",
                nxwinCursorWidth, nxwinCursorHeight);
    #endif
  }

#endif

  /* KDrive does winCreateDefColormap right after miDCInitialize */
  /* Create a default colormap */
#if CYGDEBUG
  ErrorF ("winFinishScreenInitFB - Calling winCreateDefColormap ()\n");
#endif
  if (!winCreateDefColormap (pScreen))
    {
      ErrorF ("winFinishScreenInitFB - Could not create colormap\n");
      return FALSE;
    }

#if !WIN_LAYER_SUPPORT
  /* Initialize the shadow framebuffer layer */
  if (pScreenInfo->dwEngine == WIN_SERVER_SHADOW_GDI
      || pScreenInfo->dwEngine == WIN_SERVER_SHADOW_DD
      || pScreenInfo->dwEngine == WIN_SERVER_SHADOW_DDNL)
    {
#if CYGDEBUG
      ErrorF ("winFinishScreenInitFB - Calling shadowInit ()\n");
#endif
      if (!shadowInit (pScreen,
		       pScreenPriv->pwinShadowUpdate,
		       NULL))
	{
	  ErrorF ("winFinishScreenInitFB - shadowInit () failed\n");
	  return FALSE;
	}
    }
#endif


  /* Handle pseudo-rootless mode */
  if (pScreenInfo->fRootless)
    {
      /* Define the WRAP macro temporarily for local use */
#define WRAP(a) \
    if (pScreen->a) { \
        pScreenPriv->a = pScreen->a; \
    } else { \
        ErrorF("null screen fn " #a "\n"); \
        pScreenPriv->a = NULL; \
    }

      /* Save a pointer to each lower-level window procedure */
      WRAP(CreateWindow);
      WRAP(DestroyWindow);
      WRAP(RealizeWindow);
      WRAP(UnrealizeWindow);
      WRAP(PositionWindow);
      WRAP(ChangeWindowAttributes);
#ifdef SHAPE
      WRAP(SetShape);
#endif

      /* Assign pseudo-rootless window procedures to be top level procedures */
      pScreen->CreateWindow = winCreateWindowPRootless;
      pScreen->DestroyWindow = winDestroyWindowPRootless;
      pScreen->PositionWindow = winPositionWindowPRootless;
      pScreen->ChangeWindowAttributes = winChangeWindowAttributesPRootless;
      pScreen->RealizeWindow = winMapWindowPRootless;
      pScreen->UnrealizeWindow = winUnmapWindowPRootless;
#ifdef SHAPE
      pScreen->SetShape = winSetShapePRootless;
#endif

      /* Undefine the WRAP macro, as it is not needed elsewhere */
#undef WRAP
    }
  /* Handle multi window mode */
  else if (pScreenInfo->fMultiWindow)
    {
      /* Define the WRAP macro temporarily for local use */
#define WRAP(a) \
    if (pScreen->a) { \
        pScreenPriv->a = pScreen->a; \
    } else { \
        ErrorF("null screen fn " #a "\n"); \
        pScreenPriv->a = NULL; \
    }

      /* Save a pointer to each lower-level window procedure */
      WRAP(CreateWindow);
      WRAP(DestroyWindow);
      WRAP(RealizeWindow);
      WRAP(UnrealizeWindow);
      WRAP(PositionWindow);
      WRAP(ChangeWindowAttributes);
      WRAP(ReparentWindow);
      WRAP(RestackWindow);
#ifdef SHAPE
      WRAP(SetShape);
#endif

      /* Assign multi-window window procedures to be top level procedures */
      pScreen->CreateWindow = winCreateWindowMultiWindow;
      pScreen->DestroyWindow = winDestroyWindowMultiWindow;
      pScreen->PositionWindow = winPositionWindowMultiWindow;
      pScreen->ChangeWindowAttributes = winChangeWindowAttributesMultiWindow;
      pScreen->RealizeWindow = winMapWindowMultiWindow;
      pScreen->UnrealizeWindow = winUnmapWindowMultiWindow;
      pScreen->ReparentWindow = winReparentWindowMultiWindow;
      pScreen->RestackWindow = winRestackWindowMultiWindow;
#ifdef SHAPE
      pScreen->SetShape = winSetShapeMultiWindow;
#endif

      /* Undefine the WRAP macro, as it is not needed elsewhere */
#undef WRAP
    }

  /* Wrap either fb's or shadow's CloseScreen with our CloseScreen */
  pScreenPriv->CloseScreen = pScreen->CloseScreen;
  pScreen->CloseScreen = pScreenPriv->pwinCloseScreen;

  /* Create a mutex for modules in seperate threads to wait for */
  iReturn = pthread_mutex_init (&pScreenPriv->pmServerStarted, NULL);
  if (iReturn != 0)
    {
      ErrorF ("winFinishScreenInitFB - pthread_mutex_init () failed: %d\n",
	      iReturn);
      return FALSE;
    }

  /* Own the mutex for modules in seperate threads */
  iReturn = pthread_mutex_lock (&pScreenPriv->pmServerStarted);
  if (iReturn != 0)
    {
      ErrorF ("winFinishScreenInitFB - pthread_mutex_lock () failed: %d\n",
	      iReturn);
      return FALSE;
    }

  /* Set the ServerStarted flag to false */
  pScreenPriv->fServerStarted = FALSE;

#if CYGDEBUG || YES
  if (pScreenInfo->fMultiWindow)
    ErrorF ("winFinishScreenInitFB - Calling winInitWM.\n");
#endif

  /* Initialize multi window mode */
  ErrorF("winFinishScreenInitFB starting winInitWM\n");
  if (pScreenInfo->fMultiWindow
      && !winInitWM (&pScreenPriv->pWMInfo,
		     &pScreenPriv->ptWMProc,
		     &pScreenPriv->pmServerStarted,
		     pScreenInfo->dwScreen))
    {
      ErrorF ("winFinishScreenInitFB - winInitWM () failed.\n");
      return FALSE;
    }
  ErrorF("winFinishScreenInitFB After winInitWM\n");
  /* FIXME: NOCLIPBOARD */
/*
#if CYGDEBUG || YES
  if (pScreenInfo->fClipboard)
    ErrorF ("winFinishScreenInitFB - Calling winInitClipboard.\n");
#endif
*/
  /* Initialize the clipboard manager */
  /*
  if (pScreenInfo->fClipboard
      && !winInitClipboard (&pScreenPriv->ptClipboardProc,
			    &pScreenPriv->pmServerStarted,
			    pScreenInfo->dwScreen))
    {
      ErrorF ("winFinishScreenInitFB - winClipboardInit () failed.\n");
      return FALSE;
    }
  */
  /* FIXME: NOCLIPBOARD */
  /* Tell the server that we are enabled */
  pScreenPriv->fEnabled = TRUE;

  /* Tell the server that we have a valid depth */
  pScreenPriv->fBadDepth = FALSE;

#ifdef NXWIN_LOGO
  nxagent_red   = 0xff0000;
  nxagent_black = 0x000000;
  nxagent_white = 0xffffff;
  nxagent_depth = pScreenInfo->dwDepth;

  r = pVisual->redMask;
  g = pVisual->greenMask;
  b = pVisual->blueMask;

  if(r && g && b) {
    for(or=0, off=0x800000; (r&(off>>or)) == 0;or++);
    for(og=0, off=0x800000; (g&(off>>og)) == 0;og++);
    for(ob=0, off=0x800000; (b&(off>>ob)) == 0;ob++);
    nxagent_red   = nxLogoColor(nxagent_red);
    nxagent_white = nxLogoColor(nxagent_white);
    nxagent_black = nxLogoColor(nxagent_black);
  }

  ErrorF ("color offset: %x %x %x\n", pVisual->offsetRed, pVisual->offsetGreen, pVisual->offsetBlue);
#endif

#ifdef NXWIN_PONG
  valPingPong = RegisterWindowMessage("PingPong");
  if(valPingPong == 0){
        ErrorF ("winFinishScreenInitFB () - failed to register message PingPong\n");
  }
#endif
#ifdef NXWIN_KILL
  valNxKill = RegisterWindowMessage("NXKILL");
  valConnected = RegisterWindowMessage("NXCONNECTED");
  valNxMessage = RegisterWindowMessage("NX_SESSION_MESSAGE");
  valKillESD   = RegisterWindowMessage("NX_KILLESD");
  
  storedProxyPid = RegisterWindowMessage("NX_STORED_PROXY_PID"); 
  isProxyRunning = RegisterWindowMessage("NX_IS_RUNNING_PROXY");
  setDisplay     = RegisterWindowMessage("NX_SET_DISPLAY");    
  stored_nxserver_version = RegisterWindowMessage("STORED_NXSERVER_VERSION"); 

  valNxAdminCommand = RegisterWindowMessage("NX_ADMIN_COMMAND");
  if(valNxKill == 0 || valConnected == 0 || valNxMessage == 0 || valKillESD == 0){
        ErrorF ("winFinishScreenInitFB () - failed to register message NXKILL\n");
  }
#endif

#if CYGDEBUG || YES
  ErrorF ("winFinishScreenInitFB - returning\n");
#endif

  return TRUE;
}


/*
 *
 *
 *
 *
 * TEST CODE BELOW - NOT USED IN NORMAL COMPILATION
 *
 *
 *
 *
 *
 */


/* See Porting Layer Definition - p. 20 */

Bool
winFinishScreenInitNativeGDI (int index,
			      ScreenPtr pScreen,
			      int argc, char **argv)
{
  winScreenPriv(pScreen);
  winScreenInfoPtr      pScreenInfo = &g_ScreenInfo[index];
  VisualPtr		pVisuals = NULL;
  DepthPtr		pDepths = NULL;
  VisualID		rootVisual = 0;
  int			nVisuals = 0, nDepths = 0, nRootDepth = 0;

  /* Ignore user input (mouse, keyboard) */
  pScreenInfo->fIgnoreInput = TRUE;

  /* Get device contexts for the screen and shadow bitmap */
  pScreenPriv->hdcScreen = GetDC (pScreenPriv->hwndScreen);
  if (pScreenPriv->hdcScreen == NULL)
    FatalError ("winFinishScreenInitNativeGDI - Couldn't get a DC\n");

  /* Init visuals */
  if (!(*pScreenPriv->pwinInitVisuals) (pScreen))
    {
      ErrorF ("winFinishScreenInitNativeGDI - pwinInitVisuals failed\n");
      return FALSE;
    }

  /* Initialize the mi visuals */
  if (!miInitVisuals (&pVisuals, &pDepths, &nVisuals, &nDepths, &nRootDepth,
		      &rootVisual,
		      ((unsigned long)1 << (pScreenInfo->dwDepth - 1)), 8,
		      TrueColor))
    {
      ErrorF ("winFinishScreenInitNativeGDI - miInitVisuals () failed\n");
      return FALSE;
    }

  /* Initialize the CloseScreen procedure pointer */
  pScreen->CloseScreen = NULL;

  /* Initialize the mi code */
  if (!miScreenInit (pScreen,
		     NULL, /* No framebuffer */
		     pScreenInfo->dwWidth, pScreenInfo->dwHeight,
		     monitorResolution, monitorResolution,
		     pScreenInfo->dwWidth,
		     nRootDepth, nDepths, pDepths, rootVisual,
		     nVisuals, pVisuals))
    {
      ErrorF ("winFinishScreenInitNativeGDI - miScreenInit failed\n");
      return FALSE;
    }

  /*
   * Register our block and wakeup handlers; these procedures
   * process messages in our Windows message queue; specifically,
   * they process mouse and keyboard input.
   */
  pScreen->BlockHandler = winBlockHandler;
  pScreen->WakeupHandler = winWakeupHandler;
  pScreen->blockData = pScreen;
  pScreen->wakeupData = pScreen;

  /* Place our save screen function */
  pScreen->SaveScreen = winSaveScreen;

  /* Pixmaps */
  pScreen->CreatePixmap = winCreatePixmapNativeGDI;
  pScreen->DestroyPixmap = winDestroyPixmapNativeGDI;

  /* Other Screen Routines */
  pScreen->QueryBestSize = winQueryBestSizeNativeGDI;
  pScreen->SaveScreen = winSaveScreen;  
  pScreen->GetImage = miGetImage;
  pScreen->GetSpans = winGetSpansNativeGDI;

  /* Window Procedures */
  pScreen->CreateWindow = winCreateWindowNativeGDI;
  pScreen->DestroyWindow = winDestroyWindowNativeGDI;
  pScreen->PositionWindow = winPositionWindowNativeGDI;
  pScreen->ChangeWindowAttributes = winChangeWindowAttributesNativeGDI;
  pScreen->RealizeWindow = winMapWindowNativeGDI;
  pScreen->UnrealizeWindow = winUnmapWindowNativeGDI;

  /* Paint window */
  pScreen->PaintWindowBackground = miPaintWindow;
  pScreen->PaintWindowBorder = miPaintWindow;
  pScreen->CopyWindow = winCopyWindowNativeGDI;

  /* Fonts */
  pScreen->RealizeFont = winRealizeFontNativeGDI;
  pScreen->UnrealizeFont = winUnrealizeFontNativeGDI;

  /* GC */
  pScreen->CreateGC = winCreateGCNativeGDI;

  /* Colormap Routines */
  pScreen->CreateColormap = miInitializeColormap;
  pScreen->DestroyColormap = (DestroyColormapProcPtr) (void (*)()) NoopDDA;
  pScreen->InstallColormap = miInstallColormap;
  pScreen->UninstallColormap = miUninstallColormap;
  pScreen->ListInstalledColormaps = miListInstalledColormaps;
  pScreen->StoreColors = (StoreColorsProcPtr) (void (*)()) NoopDDA;
  pScreen->ResolveColor = miResolveColor;

  /* Bitmap */
  pScreen->BitmapToRegion = winPixmapToRegionNativeGDI;

  ErrorF ("winFinishScreenInitNativeGDI - calling miDCInitialize\n");

  /* Set the default white and black pixel positions */
  pScreen->whitePixel = pScreen->blackPixel = (Pixel) 0;

  /* Initialize the cursor */

#ifdef NXWIN_OLD_CURSORS

  if (!miDCInitialize (pScreen, &g_winPointerCursorFuncs))
    {
      ErrorF ("winFinishScreenInitNativeGDI - miDCInitialize failed\n");
      return FALSE;
    }

#else

  if (nxwinOldCursors == 1)
  {
    if (!miDCInitialize (pScreen, &g_winPointerCursorFuncs))
    {
      ErrorF ("winFinishScreenInitNativeGDI - miDCInitialize failed\n");
      return FALSE;
    }

  }
  else
  {
    miPointerInitialize (pScreen, &nxwinPointerSpriteFuncs,
                               &g_winPointerCursorFuncs, 1);
  }

#endif
  
  /* Create a default colormap */
  if (!miCreateDefColormap (pScreen))
    {
        ErrorF ("winFinishScreenInitNativeGDI - miCreateDefColormap () "
		"failed\n");
	return FALSE;
    }

  ErrorF ("winFinishScreenInitNativeGDI - miCreateDefColormap () "
	  "returned\n");
  
  /* mi doesn't use a CloseScreen procedure, so no need to wrap */
  pScreen->CloseScreen = pScreenPriv->pwinCloseScreen;

  /* Tell the server that we are enabled */
  pScreenPriv->fEnabled = TRUE;

  ErrorF ("winFinishScreenInitNativeGDI - Successful addition of "
	  "screen %08x\n",
	  pScreen);

  return TRUE;
}


/* See Porting Layer Definition - p. 33 */
Bool
winSaveScreen (ScreenPtr pScreen, int on)
{
  return TRUE;
}


PixmapPtr
winGetWindowPixmap (WindowPtr pwin)
{
  ErrorF ("winGetWindowPixmap ()\n");
  return NULL;
}


void
winSetWindowPixmap (WindowPtr pwin, PixmapPtr pPix)
{
  ErrorF ("winSetWindowPixmap ()\n");
}
