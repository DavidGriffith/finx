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
 * Authors:	Kensuke Matsuzaki
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/winmultiwindowwindow.c,v 1.1 2003/02/12 15:01:38 alanh Exp $ */

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

#include "win.h"
#include "dixevents.h"
#include "winclass.h"
#include "winmultstack.h"
#include "windowstr.h"
#include "propertyst.h"

#ifdef NXWIN_MULTIWINDOW
extern Bool nxwinMultiwindow;
extern pthread_mutex_t nxwinMultiwindowMutex;
extern pthread_mutex_t nxwinMultStackMutex;

extern pthread_mutex_t nxwinMultStackMutex;
#endif

extern int nxwinSetWindowTrap;

#ifdef NXWIN_CLIPBOARD
extern int nxwinButtonDown;
#endif

extern MultStackQueuePtr pMultStackQueue;

#ifndef NXWIN_OLD_CURSORS

extern nxwinOldCursors;
extern HCURSOR nxwinCurrentCursorHandle;

#endif

/*
 * Prototypes for local functions
 */

static void
winCreateWindowsWindow (WindowPtr pWin);

static void
winDestroyWindowsWindow (WindowPtr pWin);

static void
winUpdateWindowsWindow (WindowPtr pWin);

static XID
winGetWindowID (WindowPtr pWin);

static void
SendConfigureNotify (WindowPtr pWin);

static
void
winUpdateRgn (WindowPtr pWindow);

static HICON 
privLoadIcon(WindowPtr pWin);


static HICON
winXIconToHICON ( PixmapPtr iconPtr, PixmapPtr maskPtr);

static void
winScaleXBitmapToWindows (int iconSize, int effBPP, PixmapPtr pixmap, unsigned char *image);
	
void
winMoveXWindow (WindowPtr, int, int);

void
winResizeXWindow (WindowPtr, int, int);
	
#ifdef SHAPE
static
void
winReshape (WindowPtr pWin);
#endif


/*
 * Local globals
 */

static UINT s_nIDPollingMouse = 2;

#if 0
static BOOL s_fMoveByX = FALSE;
#endif

/*
 * External globals
 */

extern int GetWindowProperty(void*, Atom, long, long, Bool, Atom, Atom*, int*, unsigned long*, unsigned long*, unsigned char**);

/*
 * Constant defines
 */


#define MOUSE_POLLING_INTERVAL 500
#define WIN_MULTIWINDOW_SHAPE YES

/*
 * Macros
 */

#define SubSend(pWin) \
    ((pWin->eventMask|wOtherEventMasks(pWin)) & SubstructureNotifyMask)

#define StrSend(pWin) \
    ((pWin->eventMask|wOtherEventMasks(pWin)) & StructureNotifyMask)

#define SubStrSend(pWin,pParent) (StrSend(pWin) || SubSend(pParent))

#define CLASS_NAME_LENGTH 512

/*
 * CreateWindow - See Porting Layer Definition - p. 37
 */

Bool
winCreateWindowMultiWindow (WindowPtr pWin)
{
  Bool			fResult = TRUE;
  winWindowPriv(pWin);

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winCreateWindowMultiWindow - pWin: %08x\n", pWin);
#endif

  /* Call any wrapped CreateWindow function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->CreateWindow)
    fResult = winGetScreenPriv(pWin->drawable.pScreen)->CreateWindow (pWin);

  /* Initialize some privates values */
  pWinPriv->hRgn = NULL;
  pWinPriv->hWnd = NULL;
  pWinPriv->pScreenPriv = winGetScreenPriv(pWin->drawable.pScreen);
  pWinPriv->fXKilled = FALSE;
  pWinPriv->fNeedRestore = FALSE;
  pWinPriv->fIconicHandled = FALSE;

  return fResult;
}


/*
 * DestroyWindow - See Porting Layer Definition - p. 37
 */

Bool
winDestroyWindowMultiWindow (WindowPtr pWin)
{
  Bool			fResult = TRUE;
  winWindowPriv(pWin);

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winDestroyWindowMultiWindow - pWin: %08x\n", pWin);
#endif

  /* Call any wrapped DestroyWindow function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->DestroyWindow)
    fResult = winGetScreenPriv(pWin->drawable.pScreen)->DestroyWindow (pWin);

  /* Flag that the window has been destroyed */
  pWinPriv->fXKilled = TRUE;

  /* Kill the MS Windows window associated with this window */
  winDestroyWindowsWindow (pWin);

  return fResult;
}


/*
 * PositionWindow - See Porting Layer Definition - p. 37
 */

Bool
winPositionWindowMultiWindow (WindowPtr pWin, int x, int y)
{
  Bool			fResult = TRUE;
  int		        iX, iY, iWidth, iHeight, iBorder;
  winWindowPriv(pWin);
  HWND hWnd = pWinPriv->hWnd;
  RECT rcNew;
  RECT rcOld;
#if CYGMULTIWINDOW_DEBUG
  RECT rcClient;
  RECT *lpRc;
#endif
  DWORD dwExStyle;
  DWORD dwStyle;

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winPositionWindowMultiWindow - pWin: %08x\n", pWin);
#endif

  /* Call any wrapped PositionWindow function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->PositionWindow)
    fResult = winGetScreenPriv(pWin->drawable.pScreen)->PositionWindow (pWin, x, y);

  /* Bail out if the Windows window handle is bad */
  if (!hWnd)
    return fResult;

  /* Get the Windows window style and extended style */
  dwExStyle = GetWindowLongPtr (hWnd, GWL_EXSTYLE);
  dwStyle = GetWindowLongPtr (hWnd, GWL_STYLE);

  /* Get the width of the X window border */
  iBorder = wBorderWidth (pWin);

  /* Get the X and Y location of the X window */
  iX = pWin->drawable.x + GetSystemMetrics (SM_XVIRTUALSCREEN);
  iY = pWin->drawable.y + GetSystemMetrics (SM_YVIRTUALSCREEN);

  /* Get the height and width of the X window */
  iWidth = pWin->drawable.width;
  iHeight = pWin->drawable.height;

  /* Store the origin, height, and width in a rectangle structure */
  SetRect (&rcNew, iX, iY, iX + iWidth, iY + iHeight);

#if CYGMULTIWINDOW_DEBUG
  lpRc = &rcNew;
  ErrorF ("winPositionWindowMultiWindow - (%d ms)drawable (%d, %d)-(%d, %d)\n",
	  GetTickCount (), lpRc->left, lpRc->top, lpRc->right, lpRc->bottom);
#endif

  /*
   * Calculate the required size of the Windows window rectangle,
   * given the size of the Windows window client area.
   */
  AdjustWindowRectEx (&rcNew, dwStyle, FALSE, dwExStyle);

  /* Get a rectangle describing the old Windows window */
  GetWindowRect (hWnd, &rcOld);

#if CYGMULTIWINDOW_DEBUG
  /* Get a rectangle describing the Windows window client area */
  GetClientRect (hWnd, &rcClient);

  lpRc = &rcNew;
  ErrorF ("winPositionWindowMultiWindow - (%d ms)rcNew (%d, %d)-(%d, %d)\n",
	  GetTickCount (), lpRc->left, lpRc->top, lpRc->right, lpRc->bottom);

  lpRc = &rcOld;
  ErrorF ("winPositionWindowMultiWindow - (%d ms)rcOld (%d, %d)-(%d, %d)\n",
	  GetTickCount (), lpRc->left, lpRc->top, lpRc->right, lpRc->bottom);

  lpRc = &rcClient;
  ErrorF ("(%d ms)rcClient (%d, %d)-(%d, %d)\n",
	  GetTickCount (), lpRc->left, lpRc->top, lpRc->right, lpRc->bottom);
#endif

  /* Check if the old rectangle and new rectangle are the same */
  if (!EqualRect (&rcNew, &rcOld))
    {
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winPositionWindowMultiWindow - Need to move\n");
#endif

      /* Change the position and dimensions of the Windows window */
      MoveWindow (hWnd,
		  rcNew.left, rcNew.top,
		  rcNew.right - rcNew.left, rcNew.bottom - rcNew.top,
		  TRUE);
    }
  else
    {
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winPositionWindowMultiWindow - Not need to move\n");
#endif
    }

  return fResult;
}


/*
 * ChangeWindowAttributes - See Porting Layer Definition - p. 37
 */

Bool
winChangeWindowAttributesMultiWindow (WindowPtr pWin, unsigned long mask)
{
  Bool			fResult = TRUE;

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winChangeWindowAttributesMultiWindow - pWin: %08x\n", pWin);
#endif

  /* Call any wrapped ChangeWindowAttributes function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->ChangeWindowAttributes)
    fResult = winGetScreenPriv(pWin->drawable.pScreen)->ChangeWindowAttributes (pWin, mask);

  /*
   * NOTE: We do not currently need to do anything here.
   */

  return fResult;
}


/*
 * UnmapWindow - See Porting Layer Definition - p. 37
 * Also referred to as UnrealizeWindow
 */

Bool
winUnmapWindowMultiWindow (WindowPtr pWin)
{
  Bool			fResult = TRUE;
  winWindowPriv(pWin);

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winUnmapWindowMultiWindow - pWin: %08x\n", pWin);
#endif

  /* Call any wrapped UnrealizeWindow function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->UnrealizeWindow)
    fResult = winGetScreenPriv(pWin->drawable.pScreen)->UnrealizeWindow (pWin);

  /* Flag that the window has been killed */
  pWinPriv->fXKilled = TRUE;

  /* Destroy the Windows window associated with this X window */
  winDestroyWindowsWindow (pWin);

  return fResult;
}


/*
 * MapWindow - See Porting Layer Definition - p. 37
 * Also referred to as RealizeWindow
 */

Bool
winMapWindowMultiWindow (WindowPtr pWin)
{
  Bool			fResult = TRUE;
  winWindowPriv(pWin);

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winMapWindowMultiWindow - pWin: %08x\n", pWin);
#endif

  /* Call any wrapped RealizeWindow function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->RealizeWindow)
    fResult = winGetScreenPriv(pWin->drawable.pScreen)->RealizeWindow (pWin);

  /* Flag that this window has not been destroyed */
  pWinPriv->fXKilled = FALSE;

  /* Refresh/redisplay the Windows window associated with this X window */
  winUpdateWindowsWindow (pWin);

#if WIN_MULTIWINDOW_SHAPE
  winReshape (pWin);
  winUpdateRgn (pWin);
#endif

  return fResult;
}


/*
 * ReparentWindow - See Porting Layer Definition - p. 42
 */

void
winReparentWindowMultiWindow (WindowPtr pWin, WindowPtr pPriorParent)
{
#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winReparentMultiWindow - pWin: %08x\n", pWin);
#endif

  /* Call any wrapped ReparentWindow function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->ReparentWindow)
    winGetScreenPriv(pWin->drawable.pScreen)->ReparentWindow (pWin,
							      pPriorParent);

  /* Update the Windows window associated with this X window */
  winUpdateWindowsWindow (pWin);
}


/*
 * RestackWindow - Shuffle the z-order of a window
 */

void
winRestackWindowMultiWindow (WindowPtr pWin, WindowPtr pOldNextSib)
{
  WindowPtr		pPrevWin;
  UINT			uFlags;
  HWND			hInsertAfter;
  winWindowPriv(pWin);

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winRestackMultiWindow - %08x\n", pWin);
#endif

  /* Call any wrapped RestackWindow function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->RestackWindow)
    winGetScreenPriv(pWin->drawable.pScreen)->RestackWindow (pWin,
							     pOldNextSib);

  /* Bail out if no window privates or window handle is invalid */
  if (!pWinPriv || !pWinPriv->hWnd)
    return;

  /* Get a pointer to our previous sibling window */
  pPrevWin = pWin->prevSib;

  /*
   * Look for a sibling window with
   * valid privates and window handle
   */
  while (pPrevWin
	 && !winGetWindowPriv(pPrevWin)
	 && !winGetWindowPriv(pPrevWin)->hWnd)
    pPrevWin = pPrevWin->prevSib;

  /* Check if we found a valid sibling */
  if (pPrevWin)
    {
      /* Valid sibling - get handle to insert window after */
      hInsertAfter = winGetWindowPriv(pPrevWin)->hWnd;
      uFlags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE;
    }
  else
    {
      /* No valid sibling - make this window the top window */
      hInsertAfter = HWND_TOP;
      uFlags = SWP_NOMOVE | SWP_NOSIZE;
    }

  if (nxwinSetWindowTrap == 0) {

    SetWindowPos (pWinPriv->hWnd,
                  hInsertAfter,
                  0, 0,
                  0, 0,
                  uFlags);
  }
}


/*
 * SetShape - See Porting Layer Definition - p. 42
 */

#ifdef SHAPE
void
winSetShapeMultiWindow (WindowPtr pWin)
{
#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winSetShapeMultiWindow - pWin: %08x\n", pWin);
#endif

  /* Call any wrapped SetShape function */
  if (winGetScreenPriv(pWin->drawable.pScreen)->SetShape)
    winGetScreenPriv(pWin->drawable.pScreen)->SetShape (pWin);

  /*
   * NOTE: We do not currently do anything here.
   */

#if WIN_MULTIWINDOW_SHAPE
  winReshape (pWin);
  winUpdateRgn (pWin);
#endif

  return;
}
#endif


/*
 * winUpdateRgn - Local function to update a Windows window region
 */

static
void
winUpdateRgn (WindowPtr pWin)
{
#if 1
  SetWindowRgn (winGetWindowPriv(pWin)->hWnd,
		winGetWindowPriv(pWin)->hRgn, TRUE);
#endif
}


/*
 * winReshape - Computes the composite clipping region for a window
 */

#ifdef SHAPE
static
void
winReshape (WindowPtr pWin)
{
  int		nRects;
  RegionRec	rrNewShape;
  BoxPtr	pShape, pRects, pEnd;
  HRGN		hRgn, hRgnRect;
  winWindowPriv(pWin);

#if CYGDEBUG
  ErrorF ("winReshape ()\n");
#endif

  /* Bail if the window is the root window */
  if (pWin->parent == NULL)
    return;

  /* Bail if the window is not top level */
  if (pWin->parent->parent != NULL)
    return;

  /* Bail if Windows window handle is invalid */
  if (pWinPriv->hWnd == NULL)
    return;

  /* Free any existing window region stored in the window privates */
  if (pWinPriv->hRgn != NULL)
    {
      DeleteObject (pWinPriv->hRgn);
      pWinPriv->hRgn = NULL;
    }

  /* Bail if the window has no bounding region defined */
  if (!wBoundingShape (pWin))
    return;

  REGION_INIT(pScreen, &rrNewShape, NullBox, 0);
  REGION_COPY(pScreen, &rrNewShape, wBoundingShape(pWin));
  REGION_TRANSLATE(pScreen,
		   &rrNewShape,
		   pWin->borderWidth,
                   pWin->borderWidth);

  nRects = REGION_NUM_RECTS(&rrNewShape);
  pShape = REGION_RECTS(&rrNewShape);

  /* Don't do anything if there are no rectangles in the region */
  if (nRects > 0)
    {
      RECT			rcClient;
      RECT			rcWindow;
      int			iOffsetX, iOffsetY;

      /* Get client rectangle */
      if (!GetClientRect (pWinPriv->hWnd, &rcClient))
	{
	  ErrorF ("winReshape - GetClientRect failed, bailing: %d\n",
		  GetLastError ());
	  return;
	}

      /* Translate client rectangle coords to screen coords */
      /* NOTE: Only transforms top and left members */
      ClientToScreen (pWinPriv->hWnd, (LPPOINT) &rcClient);

      /* Get window rectangle */
      if (!GetWindowRect (pWinPriv->hWnd, &rcWindow))
	{
	  ErrorF ("winReshape - GetWindowRect failed, bailing: %d\n",
		  GetLastError ());
	  return;
	}

      /* Calculate offset from window upper-left to client upper-left */
      iOffsetX = rcClient.left - rcWindow.left;
      iOffsetY = rcClient.top - rcWindow.top;

      /* Create initial Windows region for title bar */
      /* FIXME: Mean, nasty, ugly hack!!! */
      hRgn = CreateRectRgn (0, 0, rcWindow.right, iOffsetY);
      if (hRgn == NULL)
	{
	  ErrorF ("winReshape - Initial CreateRectRgn (%d, %d, %d, %d) "
		  "failed: %d\n",
		  0, 0, rcWindow.right, iOffsetY, GetLastError ());
	}

      /* Loop through all rectangles in the X region */
      for (pRects = pShape, pEnd = pShape + nRects; pRects < pEnd; pRects++)
        {
	  /* Create a Windows region for the X rectangle */
	  hRgnRect = CreateRectRgn (pRects->x1 + iOffsetX - 1,
				    pRects->y1 + iOffsetY - 1,
				    pRects->x2 + iOffsetX - 1,
				    pRects->y2 + iOffsetY - 1);
	  if (hRgnRect == NULL)
	    {
	      ErrorF ("winReshape - Loop CreateRectRgn (%d, %d, %d, %d) "
		      "failed: %d\n"
		      "\tx1: %d x2: %d xOff: %d y1: %d y2: %d yOff: %d\n",
		      pRects->x1 + iOffsetX - 1,
		      pRects->y1 + iOffsetY - 1,
		      pRects->x2 + iOffsetX - 1,
		      pRects->y2 + iOffsetY - 1,
		      GetLastError (),
		      pRects->x1, pRects->x2, iOffsetX,
		      pRects->y1, pRects->y2, iOffsetY);
	    }

	  /* Merge the Windows region with the accumulated region */
	  if (CombineRgn (hRgn, hRgn, hRgnRect, RGN_OR) == ERROR)
	    {
	      ErrorF ("winReshape - CombineRgn () failed: %d\n",
		      GetLastError ());
	    }

	  /* Delete the temporary Windows region */
	  DeleteObject (hRgnRect);
        }

      /* Save a handle to the composite region in the window privates */
      pWinPriv->hRgn = hRgn;
    }

  REGION_UNINIT(pScreen, &rrNewShape);

  return;
}
#endif

static WindowPtr
winGetTransientForWindow (WindowPtr pWin)
{
  WindowPtr retWin;
  PropertyPtr pProp;
  pointer data;

  retWin = pWin -> prevSib;

  while (retWin)
  {
    pProp = wUserProps(retWin);

    while (pProp)
    {
      data = pProp -> data;

      if (!strcmp(NameForAtom(pProp -> propertyName), "WM_TRANSIENT_FOR"))
      {
        if (winGetWindowID(pWin) == *(XID*)data && IsWindow(winGetWindowPriv(retWin)->hWnd))
        {
          return retWin;
        }
        break;
      }
      pProp = pProp -> next;
    }
    retWin = retWin -> prevSib;
  }

  retWin = pWin -> nextSib;

  while (retWin)
  {
    pProp = wUserProps(retWin);

    while (pProp)
    {
      data = pProp -> data;

      if (!strcmp(NameForAtom(pProp -> propertyName), "WM_TRANSIENT_FOR"))
      {
        if (winGetWindowID(pWin) == *(XID*)data && IsWindow(winGetWindowPriv(retWin)->hWnd))
        {
          return retWin;
        }
        break;
      }
      pProp = pProp -> next;
    }
    retWin = retWin -> nextSib;
  }
  return NULL;
}

static WindowPtr
winIsTransientForWindow (WindowPtr pWin)
{
  PropertyPtr pProp;
  pointer data;

  pProp = wUserProps(pWin);

  while (pProp)
  {
    if (!strcmp(NameForAtom(pProp->propertyName), "WM_TRANSIENT_FOR"))
    {
      data = pProp->data;
      return (WindowPtr) SecurityLookupWindow(*(XID*)data, wClient(pWin), SecurityReadAccess);
    }
    pProp = pProp->next;
  }
  return NULL;
}

/*
 * winTopLevelWindowProc - Window procedure for all top-level Windows windows.
 */

LRESULT CALLBACK
winTopLevelWindowProc (HWND hwnd, UINT message,
		       WPARAM wParam, LPARAM lParam)
{
  POINT			ptMouse;
  HDC			hdcUpdate;
  PAINTSTRUCT		ps;
  WindowPtr		pWin = NULL;
  winPrivWinPtr	        pWinPriv = NULL;
  ScreenPtr		s_pScreen = NULL;
  winPrivScreenPtr	s_pScreenPriv = NULL;
  winScreenInfo		*s_pScreenInfo = NULL;
  HWND			hwndScreen = NULL;
  DrawablePtr		pDraw = NULL;
  int		        iX, iY, iWidth, iHeight, iBorder;
  winWMMessageRec	wmMsg;
  static Bool		s_fTracking = FALSE;
  static Bool           s_fCursor = TRUE;
  WindowPtr             transientForWin;

  /* Check if the Windows window property for our X window pointer is valid */
  if ((pWin = GetProp (hwnd, WIN_WINDOW_PROP)) != NULL)
    {
      /* Our X window pointer is valid */

      /* Get pointers to the drawable and the screen */
      pDraw		= &pWin->drawable;
      s_pScreen		= pWin->drawable.pScreen;

      /* Get a pointer to our window privates */
      pWinPriv		= winGetWindowPriv(pWin);

      /* Get pointers to our screen privates and screen info */
      s_pScreenPriv	= pWinPriv->pScreenPriv;
      s_pScreenInfo	= s_pScreenPriv->pScreenInfo;

      /* Get the handle for our screen-sized window */
      /* NOTE: This will be going away at some point, right?  Harold Hunt - 2003/01/15 */
      hwndScreen	= s_pScreenPriv->hwndScreen;

      /* */
      wmMsg.msg		= 0;
      wmMsg.hwndWindow	= hwnd;
      wmMsg.iWindow	= (Window)GetProp (hwnd, WIN_WID_PROP);
      wmMsg.pWin        = pWin;

#if 1
      wmMsg.iX		= pWinPriv->iX;
      wmMsg.iY		= pWinPriv->iY;
      wmMsg.iWidth	= pWinPriv->iWidth;
      wmMsg.iHeight	= pWinPriv->iHeight;
#else
      wmMsg.iX		= pDraw.x;
      wmMsg.iY		= pDraw.y;
      wmMsg.iWidth	= pDraw.width;
      wmMsg.iHeight	= pDraw.height;
#endif


#if 0
      /*
       * Print some debugging information
       */

      ErrorF ("hWnd %08X\n", hwnd);
      ErrorF ("pWin %08X\n", pWin);
      ErrorF ("pDraw %08X\n", pDraw);
      ErrorF ("\ttype %08X\n", pWin->drawable.type);
      ErrorF ("\tclass %08X\n", pWin->drawable.class);
      ErrorF ("\tdepth %08X\n", pWin->drawable.depth);
      ErrorF ("\tbitsPerPixel %08X\n", pWin->drawable.bitsPerPixel);
      ErrorF ("\tid %08X\n", pWin->drawable.id);
      ErrorF ("\tx %08X\n", pWin->drawable.x);
      ErrorF ("\ty %08X\n", pWin->drawable.y);
      ErrorF ("\twidth %08X\n", pWin->drawable.width);
      ErrorF ("\thenght %08X\n", pWin->drawable.height);
      ErrorF ("\tpScreen %08X\n", pWin->drawable.pScreen);
      ErrorF ("\tserialNumber %08X\n", pWin->drawable.serialNumber);
      ErrorF ("g_iWindowPrivateIndex %d\n", g_iWindowPrivateIndex);
      ErrorF ("pWinPriv %08X\n", pWinPriv);
      ErrorF ("s_pScreenPriv %08X\n", s_pScreenPriv);
      ErrorF ("s_pScreenInfo %08X\n", s_pScreenInfo);
      ErrorF ("hwndScreen %08X\n", hwndScreen);
#endif
    }



  /* Branch on message type */
  switch (message)
    {
    case WM_CREATE:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_CREATE\n");
#endif

      /* */
      SetProp (hwnd,
	       WIN_WINDOW_PROP,
	       (HANDLE)((LPCREATESTRUCT) lParam)->lpCreateParams);

      /* */
      SetProp (hwnd,
	       WIN_WID_PROP,
	       (HANDLE)winGetWindowID (((LPCREATESTRUCT) lParam)->lpCreateParams));
      return 0;

    case WM_PAINT:
      /* Only paint if our window handle is valid */
      if (hwndScreen == NULL)
	break;

      /* BeginPaint gives us an hdc that clips to the invalidated region */
      hdcUpdate = BeginPaint (hwnd, &ps);

#if 0
      /* NOTE: Doesn't appear to be used - Harold Hunt - 2003/01/15 */
      /* Get the dimensions of the client area */
      GetClientRect (hwnd, &rcClient);
#endif

      /* Get the position and dimensions of the window */
      iBorder = wBorderWidth (pWin);
      iX = pWin->drawable.x;
      iY = pWin->drawable.y;
      iWidth = pWin->drawable.width;
      iHeight = pWin->drawable.height;

      /* Try to copy from the shadow buffer */
      if (!BitBlt (hdcUpdate,
		   0, 0,
		   iWidth, iHeight,
		   s_pScreenPriv->hdcShadow,
		   iX, iY,
		   SRCCOPY))
	{
	  LPVOID lpMsgBuf;

	  /* Display a fancy error message */
	  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
			 FORMAT_MESSAGE_FROM_SYSTEM |
			 FORMAT_MESSAGE_IGNORE_INSERTS,
			 NULL,
			 GetLastError (),
			 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			 (LPTSTR) &lpMsgBuf,
			 0, NULL);

	  ErrorF ("winTopLevelWindowProc - BitBlt failed: %s\n",
		  (LPSTR)lpMsgBuf);
	  LocalFree (lpMsgBuf);
	}

      /* EndPaint frees the DC */
      EndPaint (hwndScreen, &ps);
      return 0;


#if 1
    case WM_MOUSEMOVE:
      /* Unpack the client area mouse coordinates */
      ptMouse.x = GET_X_LPARAM(lParam);
      ptMouse.y = GET_Y_LPARAM(lParam);

      /* Translate the client area mouse coordinates to screen coordinates */
      ClientToScreen (hwnd, &ptMouse);

      /* We can't do anything without privates */
      if (s_pScreenPriv == NULL || s_pScreenInfo->fIgnoreInput)
	break;

      /* Has the mouse pointer crossed screens? */
      if (s_pScreen != miPointerCurrentScreen ())
	miPointerSetNewScreen (s_pScreenInfo->dwScreen,
			       ptMouse.x -
			       GetSystemMetrics(SM_XVIRTUALSCREEN) -
			       s_pScreenInfo->dwXOffset,
			       ptMouse.y -
			       GetSystemMetrics(SM_YVIRTUALSCREEN) -
			       s_pScreenInfo->dwYOffset);

      /* Are we tracking yet? */
      if (!s_fTracking)
	{
	  TRACKMOUSEEVENT		tme;

	  /* Setup data structure */
	  ZeroMemory (&tme, sizeof (tme));
	  tme.cbSize = sizeof (tme);
	  tme.dwFlags = TME_LEAVE;
	  tme.hwndTrack = hwnd;

	  /* Call the tracking function */
	  if (!(*g_fpTrackMouseEvent) (&tme))
	    ErrorF ("winTopLevelWindowProc - _TrackMouseEvent failed\n");

	  /* Flag that we are tracking now */
	  s_fTracking = TRUE;
	}

      /* Hide or show the Windows mouse cursor */
      if (s_fCursor)
	{
	  /* Hide Windows cursor */
	  s_fCursor = FALSE;
          #ifdef NXWIN_OLD_CURSORS
	  ShowCursor (FALSE);
          #else
          if (nxwinOldCursors == 1)
          {
	    ShowCursor (FALSE);
          }
          else
          { 
            SetCursor(nxwinCurrentCursorHandle);
          }
          #endif

	  KillTimer (hwnd, s_nIDPollingMouse);
	}
/*
FIXME
        else
        {
          s_fCursor = TRUE;
          ShowCursor (TRUE);
        }
*/
      /* Deliver absolute cursor position to X Server */

      miPointerAbsoluteCursor (ptMouse.x - GetSystemMetrics(SM_XVIRTUALSCREEN) -
			       s_pScreenInfo->dwXOffset,
			       ptMouse.y - GetSystemMetrics(SM_YVIRTUALSCREEN) -
			       s_pScreenInfo->dwYOffset,
			       g_c32LastInputEventTime = GetTickCount ());
      return 0;
     case WM_KILLFOCUS:
     {
       extern  void nxwinLostFocus();

       winKeybdReleaseKeys();

       nxwinLostFocus();
       return 0;
     }
     case WM_SETFOCUS:
     {
       extern void nxwinGotFocus(WPARAM wParam);
       nxwinGotFocus(wParam);
       return 0; 
     } 
     case WM_NCMOUSEMOVE:
      /*
       * We break instead of returning 0 since we need to call
       * DefWindowProc to get the mouse cursor changes
       * and min/max/close button highlighting in Windows XP.
       * The Platform SDK says that you should return 0 if you
       * process this message, but it fails to mention that you
       * will give up any default functionality if you do return 0.
       */

      /* We can't do anything without privates */
      if (s_pScreenPriv == NULL || s_pScreenInfo->fIgnoreInput)
	break;

      /* Non-client mouse movement, show Windows cursor */
      if (!s_fCursor)
	{
	  s_fCursor = TRUE;
	  ShowCursor (TRUE);
	  SetTimer (hwnd, s_nIDPollingMouse, MOUSE_POLLING_INTERVAL, NULL);
	}
      break;

    case WM_MOUSELEAVE:
      /* Mouse has left our client area */

      /* Flag that we are no longer tracking */
      s_fTracking = FALSE;

      /* Show the mouse cursor, if necessary */
      if (!s_fCursor)
	{
	  s_fCursor = TRUE;
	  ShowCursor (TRUE);
	  SetTimer (hwnd, s_nIDPollingMouse, MOUSE_POLLING_INTERVAL, NULL);
	}
      return 0;

    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
      if (s_pScreenPriv == NULL || s_pScreenInfo->fIgnoreInput)
	break;
#ifdef NXWIN_CLIPBOARD
      nxwinButtonDown = 1;
#endif
      return winMouseButtonsHandle (s_pScreen, ButtonPress, Button1, wParam);

    case WM_LBUTTONUP:
      if (s_pScreenPriv == NULL || s_pScreenInfo->fIgnoreInput)
	break;
#ifdef NXWIN_CLIPBOARD
      nxwinButtonDown = 0;
#endif
      return winMouseButtonsHandle (s_pScreen, ButtonRelease, Button1, wParam);

    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
      if (s_pScreenPriv == NULL || s_pScreenInfo->fIgnoreInput)
	break;
      return winMouseButtonsHandle (s_pScreen, ButtonPress, Button2, wParam);

    case WM_MBUTTONUP:
      if (s_pScreenPriv == NULL || s_pScreenInfo->fIgnoreInput)
	break;
      return winMouseButtonsHandle (s_pScreen, ButtonRelease, Button2, wParam);

    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
      if (s_pScreenPriv == NULL || s_pScreenInfo->fIgnoreInput)
	break;
      return winMouseButtonsHandle (s_pScreen, ButtonPress, Button3, wParam);

    case WM_RBUTTONUP:
      if (s_pScreenPriv == NULL || s_pScreenInfo->fIgnoreInput)
	break;
      return winMouseButtonsHandle (s_pScreen, ButtonRelease, Button3, wParam);

#else

    case WM_MOUSEMOVE:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_MOUSEMOVE*\n");
#endif

      /* Unpack the client area mouse coordinates */
      ptMouse.x = GET_X_LPARAM(lParam);
      ptMouse.y = GET_Y_LPARAM(lParam);

      /* Translate the client area mouse coordinates to screen coordinates */
      ClientToScreen (hwnd, &ptMouse);

      /* Pass the message to the root window */
      SendMessage (hwndScreen, message, wParam, MAKELONG(ptMouse.x, ptMouse.y));
      return  0;

    case WM_NCMOUSEMOVE:
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSELEAVE:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_*BUTTON*\n");
#endif

      /* Pass the message to the root window */
      SendMessage(hwndScreen, message, wParam, MAKELONG(ptMouse.x, ptMouse.y));
      return  0;
#endif

    case WM_MOUSEWHEEL:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_MOUSEWHEEL\n");
#endif

      /* Pass the message to the root window */
      SendMessage(hwndScreen, message, wParam, lParam);
      return 0;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSDEADCHAR:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_DEADCHAR:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_*KEY*\n");
#endif

      /* Pass the message to the root window */
      SendMessage (hwndScreen, message, wParam, lParam);
      return 0;

    case WM_HOTKEY:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_HOTKEY\n");
#endif

      /* Pass the message to the root window */
      SendMessage (hwndScreen, message, wParam, lParam);
      return 0;


#if 1
    case WM_ACTIVATE:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_ACTIVATE\n");
#endif

      /* Pass the message to the root window */
      SendMessage (hwndScreen, message, wParam, lParam);

      /* Bail if inactivating */
      if (LOWORD(wParam) == WA_INACTIVE)
	return 0;

      /* Check if the current window is the active window in Windows */
      if (GetActiveWindow () == hwnd)
      {

        /*
         * Avoid restack if we find a window which has
         * WM_TRANSIENT_FOR property set to this window.
         */

        transientForWin = (WindowPtr) winGetTransientForWindow(pWin);

        if (transientForWin &&
               ((winGetWindowPriv(transientForWin)) -> fXKilled) == 0 &&
                   ((winGetWindowPriv(pWin)) -> fXKilled) == 0)
        {

          /* Tell our Window Manager thread to raise the window */
          wmMsg.msg = WM_WM_RAISE;
          winSendMessageToWM (s_pScreenPriv->pWMInfo, &wmMsg);

          SetWindowPos(winGetWindowPriv(transientForWin)->hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        else
        {
	  /* Tell our Window Manager thread to raise the window */
	  wmMsg.msg = WM_WM_RAISE;
	  winSendMessageToWM (s_pScreenPriv->pWMInfo, &wmMsg);
        }
      }

      /* Tell our Window Manager thread to activate the window */
      wmMsg.msg = WM_WM_ACTIVATE;
      winSendMessageToWM (s_pScreenPriv->pWMInfo, &wmMsg);

      return 0;

    case WM_ACTIVATEAPP:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_ACTIVATEAPP\n");
#endif

      /* Pass the message to the root window */
      SendMessage (hwndScreen, message, wParam, lParam);
      return 0;
#endif


    case WM_CLOSE:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_CLOSE\n");
#endif
      
      /* Branch on if the window was killed in X already */
      if (pWinPriv->fXKilled)
        {
	  /* Window was killed, go ahead and destroy the window */
	  DestroyWindow (hwnd);
	}
      else
	{
	  /* Tell our Window Manager thread to kill the window */
	  wmMsg.msg = WM_WM_KILL;
	  winSendMessageToWM (s_pScreenPriv->pWMInfo, &wmMsg);
	}
      return 0;

    case WM_DESTROY:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_DESTROY\n");
#endif
      /* Branch on if the window was killed in X already */
      if (pWinPriv && !pWinPriv->fXKilled)
	{

	  /* Tell our Window Manager thread to kill the window */
	  wmMsg.msg = WM_WM_KILL;
	  winSendMessageToWM (s_pScreenPriv->pWMInfo, &wmMsg);
	}

#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_DESTROY\n");
#endif
      break;

    case WM_MOVE:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_MOVE - %d ms\n", GetTickCount ());
#endif

      /* Bail if Windows window is not actually moving */
      if (pWinPriv->iX == (short) LOWORD(lParam)
	  && pWinPriv->iY == (short) HIWORD(lParam))
	break;

      /* Also bail if we're maximizing, we'll do the whole thing in WM_SIZE */
      {
        WINDOWPLACEMENT windPlace;
        windPlace.length = sizeof(WINDOWPLACEMENT);

        /* Get current window placement */
        GetWindowPlacement(hwnd, &windPlace);

        /* Bail if maximizing */
        if (windPlace.showCmd == SW_MAXIMIZE
            || windPlace.showCmd == SW_SHOWMAXIMIZED)
          break;
      }

      /* Get new position */
      pWinPriv->iX = (short) LOWORD(lParam);
      pWinPriv->iY = (short) HIWORD(lParam);

#if CYGMULTIWINDOW_DEBUG
      ErrorF ("\t(%d, %d)\n", pWinPriv->iX, pWinPriv->iY);
#endif

      winMoveXWindow (pWin,
                      (LOWORD(lParam) - wBorderWidth (pWin)
                       - GetSystemMetrics (SM_XVIRTUALSCREEN)),
                      (HIWORD(lParam) - wBorderWidth (pWin)
                       - GetSystemMetrics (SM_YVIRTUALSCREEN)));
      return 0;

    case WM_SHOWWINDOW:
#if CYGMULTIWINDOW_DEBUG
      ErrorF("winTopLevelWindowProc: case(WM_SHOWWINDOW)\n");
#endif
      /* Bail out if the window is being hidden */
      if (!wParam)
	return 0;

      /* Tell X to map the window */
      MapWindow (pWin, wClient(pWin));

      /* */
      if (!pWin->overrideRedirect)
	{
	  DWORD		dwExStyle;
	  DWORD		dwStyle;
	  RECT		rcNew;
	  int		iDx, iDy;

	  /* Flag that this window needs to be made active when clicked */
	  SetProp (hwnd, WIN_NEEDMANAGE_PROP, (HANDLE) 1);

	  /* Get the standard and extended window style information */
	  dwExStyle = GetWindowLongPtr (hwnd, GWL_EXSTYLE);
	  dwStyle = GetWindowLongPtr (hwnd, GWL_STYLE);

	  /* */
	  if (dwExStyle != WS_EX_APPWINDOW)
	    {
	      /* Setup a rectangle with the X window position and size */
	      SetRect (&rcNew,
		       pWinPriv->iX,
		       pWinPriv->iY,
		       pWinPriv->iX + pWinPriv->iWidth,
		       pWinPriv->iY + pWinPriv->iHeight);

#if 0
	      ErrorF ("winTopLevelWindowProc - (%d, %d)-(%d, %d)\n",
		      rcNew.left, rcNew.top,
		      rcNew.right, rcNew.bottom);
#endif

	      /* */
	      AdjustWindowRectEx (&rcNew,
				  WS_POPUP | WS_SIZEBOX | WS_OVERLAPPEDWINDOW,
				  FALSE,
				  WS_EX_APPWINDOW);

	      /* Calculate position deltas */
	      iDx = pWinPriv->iX - rcNew.left;
	      iDy = pWinPriv->iY - rcNew.top;

	      /* Calculate new rectangle */
	      rcNew.left += iDx;
	      rcNew.right += iDx;
	      rcNew.top += iDy;
	      rcNew.bottom += iDy;

#if 0
	      ErrorF ("winTopLevelWindowProc - (%d, %d)-(%d, %d)\n",
		      rcNew.left, rcNew.top,
		      rcNew.right, rcNew.bottom);
#endif

	      /* Set the window extended style flags */
	      SetWindowLongPtr (hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);

	      /* Set the window standard style flags */
	      SetWindowLongPtr (hwnd, GWL_STYLE, WS_POPUP | WS_SIZEBOX | WS_OVERLAPPEDWINDOW);

	      /* Positon the Windows window */
	      SetWindowPos (hwnd, HWND_TOP,
			    rcNew.left, rcNew.top,
			    rcNew.right - rcNew.left, rcNew.bottom - rcNew.top,
			    SWP_NOMOVE | SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOACTIVATE);

	      /* Bring the Window window to the foreground */
	      SetForegroundWindow (hwnd);
	    }
	}

      /* Setup the Window Manager message */
      wmMsg.msg = WM_WM_MAP;
      wmMsg.iWidth = pWinPriv->iWidth;
      wmMsg.iHeight = pWinPriv->iHeight;

      /* Tell our Window Manager thread to map the window */
      winSendMessageToWM (s_pScreenPriv->pWMInfo, &wmMsg);

      /* Setup the Window Manager message */
      wmMsg.msg = WM_WM_RAISE;

      /* Tell our Window Manager thread to raise the window */
      winSendMessageToWM (s_pScreenPriv->pWMInfo, &wmMsg);

      return 0;

    case WM_SIZE:
      /* see dix/window.c */
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_SIZE - %d ms\n", GetTickCount ());
#endif

      /* Branch on type of resizing occurring */
      switch (wParam)
	{
	case SIZE_MINIMIZED:
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\tSIZE_MINIMIZED\n");
#endif

	  wmMsg.msg = WM_WM_LOWER;

	  /* Tell our Window Manager thread to lower the window */
	  winSendMessageToWM (s_pScreenPriv->pWMInfo, &wmMsg);

          /*
           * If this window has WM_TRANSIENT_FOR property, also the window
           * that WM_TRANSIENT_FOR is set to must be minimized.
           */

          WindowPtr pW;
          pW = winIsTransientForWindow(pWin);

          if (pW)
          {
            WINDOWPLACEMENT windPlace;
            windPlace.length = sizeof(WINDOWPLACEMENT);

            GetWindowPlacement(winGetWindowPriv(pW)->hWnd, &windPlace);

            windPlace.showCmd = SW_MINIMIZE;
            SetWindowPlacement(winGetWindowPriv(pW)->hWnd, &windPlace);
          }

	  break;

	case SIZE_RESTORED:
	case SIZE_MAXIMIZED:
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("SIZE_RESTORED || SIZE_MAXIMIZED\n");
#endif
	  /* FIXME: perhaps it can be Optimized 
	   *        for SIZE_MAXIMIZED doesn't need winResizeXWindow */
	  if (pWinPriv->iWidth == (short) LOWORD(lParam)
	      && pWinPriv->iHeight == (short) HIWORD(lParam)
	      && wParam == SIZE_RESTORED)
          {

            /*
             * If we find a window which has WM_TRANSIENT_FOR property
             * set to this window, also such a window must be restored.
             */

            transientForWin = (WindowPtr) winGetTransientForWindow(pWin);

            if (transientForWin)
            {
              WINDOWPLACEMENT windPlace;
              windPlace.length = sizeof(WINDOWPLACEMENT);

              GetWindowPlacement(winGetWindowPriv(transientForWin)->hWnd, &windPlace);

              windPlace.showCmd = SW_RESTORE;
              SetWindowPlacement(winGetWindowPriv(transientForWin)->hWnd, &windPlace);
            }

	    break;
          }
	  
	  /* Get the dimensions of the resized Windows window  */
	  pWinPriv->iWidth = (short) LOWORD(lParam);
	  pWinPriv->iHeight = (short) HIWORD(lParam);

#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\t(%d, %d)\n", pWinPriv->iWidth, pWinPriv->iHeight);
#endif

	  /*
	   * If we're maximizing the window has been moved to upper left
	   * of current screen.  Now it is safe for X to know about this.
	   */
	  if (wParam == SIZE_MAXIMIZED)
	    {
	      POINT		ptHome;

	      /* Flag that we are being maximized and store info for restore */
	      pWinPriv->fNeedRestore = TRUE;
	      pWinPriv->ptRestore.x = pWinPriv->iX;
	      pWinPriv->ptRestore.y = pWinPriv->iY;
	     
	      /* Get screen location of window root */
	      ptHome.x = 0;
	      ptHome.y = 0;
	      ClientToScreen (hwnd, &ptHome);

	      /* Map from screen (-X,-Y) to (0,0) root coords */
	      winMoveXWindow (pWin,
			      ptHome.x - wBorderWidth (pWin)
			      - GetSystemMetrics (SM_XVIRTUALSCREEN),
			      ptHome.y - wBorderWidth (pWin)
			      - GetSystemMetrics (SM_YVIRTUALSCREEN));
	    }
	  else if (wParam == SIZE_RESTORED && pWinPriv->fNeedRestore)
	    {
	      /* If need restore and !maximized then move to cached position */
	      WINDOWPLACEMENT windPlace;

	      windPlace.length = sizeof (WINDOWPLACEMENT);

	      GetWindowPlacement (hwnd, &windPlace);

	      if (windPlace.showCmd != SW_MAXIMIZE
		  && windPlace.showCmd != SW_SHOWMAXIMIZED)
		{
		  pWinPriv->fNeedRestore = FALSE;
		  winMoveXWindow (pWin,
				  pWinPriv->ptRestore.x  - wBorderWidth (pWin)
				  - GetSystemMetrics (SM_XVIRTUALSCREEN),
				  pWinPriv->ptRestore.y - wBorderWidth (pWin)
				  - GetSystemMetrics (SM_YVIRTUALSCREEN));
		}
	    }

	  /* Perform the resize and notify the X client */
	  winResizeXWindow (pWin,
			    (short) LOWORD(lParam),
			    (short) HIWORD(lParam));
	  break;


	default:
	  break;
	}
      return 0;

    case WM_MOUSEACTIVATE:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_MOUSEACTIVATE\n");
#endif

      /*
       * Avoid restack if we find a window which has
       * WM_TRANSIENT_FOR property set to this window.
       */

      transientForWin = (WindowPtr) winGetTransientForWindow(pWin);

      if (transientForWin)
      {
        SetWindowPos(pWinPriv->hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        SetWindowPos(winGetWindowPriv(transientForWin)->hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        return MA_NOACTIVATEANDEAT;
      }

      /* Check if this window needs to be made active when clicked */
      if (!GetProp (pWinPriv->hWnd, WIN_NEEDMANAGE_PROP))
	{
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("winTopLevelWindowProc - WM_MOUSEACTIVATE - MA_NOACTIVATE\n");
#endif

	  /* */
	  return MA_NOACTIVATE;
	}
      break;

    case WM_TIMER:
#if CYGMULTIWINDOW_DEBUG
      ErrorF ("winTopLevelWindowProc - WM_TIMER - %d ms\n", GetTickCount ());
#endif

      /* Branch on the type of timer event that fired */
      if (wParam == s_nIDPollingMouse)
	{
	  POINT		point;

	  /* Get the current position of the mouse cursor */
	  GetCursorPos (&point);

	  /* Deliver absolute cursor position to X Server */
	  miPointerAbsoluteCursor (point.x - GetSystemMetrics(SM_XVIRTUALSCREEN),
				   point.y - GetSystemMetrics(SM_YVIRTUALSCREEN),
				   g_c32LastInputEventTime = GetTickCount ());
	}
      else
	{
	  ErrorF ("winTopLevelWindowProc - Unknown WM_TIMER\n");
	}
      return 0;

    default:
      break;
    }

  return DefWindowProc (hwnd, message, wParam, lParam);
}


/*
 * winCreateWindowsWindow - Create a Windows window associated with an X window
 */

static void
winCreateWindowsWindow (WindowPtr pWin)
{
  int                   iX, iY;
  int			iWidth;
  int			iHeight;
  int                   iBorder;
  HWND			hWnd;
  WNDCLASS		wc;
  winWindowPriv(pWin);
  HICON                 hIcon;
  char                  pszClass[CLASS_NAME_LENGTH], pszWindowID[12];
  char                  *res_name, *res_class, *res_role;
  static int            s_iWindowID = 0;

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winCreateWindowsWindow - pWin: %08x\n", pWin);
#endif

  hIcon = privLoadIcon(pWin);

  /* Set standard class name prefix so we can identify window easily */
  strncpy (pszClass, WINDOW_CLASS_X, strlen (WINDOW_CLASS_X));

  if (GetClassHint (pWin, &res_name, &res_class))
    {
      strncat (pszClass, "-", 1);
      strncat (pszClass, res_name, CLASS_NAME_LENGTH - strlen (pszClass));
      strncat (pszClass, "-", 1);
      strncat (pszClass, res_class, CLASS_NAME_LENGTH - strlen (pszClass));

      /* Check if a window class is provided by the WM_WINDOW_ROLE property,
 *     * if not use the WM_CLASS information.
 *     * For further information see:
 *     * http://tronche.com/gui/x/icccm/sec-5.html
 *     */

      if (GetWindowRole (pWin, &res_role) )
        {
          strcat (pszClass, "-");
          strcat (pszClass, res_role);
          free (res_role);
        }

      free (res_name);
      free (res_class);
    }

  /* Add incrementing window ID to make unique class name */
  sprintf (pszWindowID, "-%x", s_iWindowID++);
  strcat (pszClass, pszWindowID);

  iBorder = wBorderWidth (pWin);

  iX = pWin->drawable.x + GetSystemMetrics(SM_XVIRTUALSCREEN);
  iY = pWin->drawable.y + GetSystemMetrics(SM_YVIRTUALSCREEN);

  iWidth = pWin->drawable.width;
  iHeight = pWin->drawable.height;

  /* Setup our window class */
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = winTopLevelWindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = g_hInstance;
  wc.hIcon = hIcon; //LoadIcon (g_hInstance, MAKEINTRESOURCE(IDI_ICON2));
  wc.hCursor = 0;
  wc.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = pszClass; //WINDOW_CLASS_X;
  RegisterClass (&wc);

  /* Create the window */
  hWnd = CreateWindowExA (WS_EX_TOOLWINDOW,			/* Extended styles */
			  pszClass, //WINDOW_CLASS_X,			/* Class name */
			  WINDOW_TITLE_X,			/* Window name */
			  WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			  iX,					/* Horizontal position */
			  iY,					/* Vertical position */
			  iWidth,				/* Right edge */
			  iHeight,				/* Bottom edge */
			  (HWND) NULL,				/* No parent or owner window */
			  (HMENU) NULL,				/* No menu */
			  GetModuleHandle (NULL),		/* Instance handle */
			  pWin);				/* ScreenPrivates */
  if (hWnd == NULL)
    {
      ErrorF ("winCreateWindowsWindow - CreateWindowExA () failed: %d\n",
	      GetLastError ());
    }

  pWinPriv->hWnd = hWnd;


  SetProp (pWinPriv->hWnd, WIN_WID_PROP, (HANDLE) winGetWindowID(pWin));

  /* Flag that this Windows window handles its own activation */
  SetProp (pWinPriv->hWnd, WIN_NEEDMANAGE_PROP, (HANDLE) 0);
#if CYGMULTIWINDOW_DEBUG
  ErrorF("Created Window : WindowID = %d\n",winGetWindowID(pWin));
#endif
}


/*
 * winDestroyWindowsWindow - Destroy a Windows window associated with an X window
 */

static void
winDestroyWindowsWindow (WindowPtr pWin)
{
  MSG			msg;
  winWindowPriv(pWin);
  char                  classname[CLASS_NAME_LENGTH];

  MultStackNodePtr p, pOld;

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winDestroyWindowsWindow\n");
#endif


  /* Bail out if the Windows window handle is invalid */
  if (pWinPriv->hWnd == NULL)
    return;

  SetProp (pWinPriv->hWnd, WIN_WINDOW_PROP, 0);

#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
    if(nxwinMultiwindow)
         ErrorF("winDestroyWindowsWindow: lock before queue access\n");
#endif
    if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultStackMutex))
         ErrorF("winDestroyWindowsWindow: pthread_mutex_lock() before queue failed\n");

    if(nxwinMultiwindow) {

      /* Delete window to destroy from queue */
      if (pMultStackQueue -> pHead != NULL) {
        if (pMultStackQueue -> pHead -> pWin == pWin) {
          pOld = pMultStackQueue -> pHead;
          pMultStackQueue -> pHead = pMultStackQueue -> pHead -> pNext;
          free(pOld);
        }
        p = pMultStackQueue -> pHead;
        while (p != NULL && p -> pNext != NULL) {
          if (p -> pNext -> pWin == pWin) {
            pOld = p -> pNext;
            p -> pNext = p -> pNext -> pNext;
            free(pOld);
            if (p -> pNext == NULL) {
              pMultStackQueue -> pTail = p;
            }
          }
          p = p -> pNext;
        }
      }
    }

#ifdef NXWIN_MULTIWINDOW_DEBUG
     if(nxwinMultiwindow)
          ErrorF("winDestroyWindowsWindow: unlock after queue access\n");
#endif
     if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultStackMutex))
          ErrorF("winDestroyWindowsWindow: pthread_mutex_unlock() after queue failed\n");
#endif

  DestroyWindow (pWinPriv->hWnd);

  pWinPriv->hWnd = NULL;

  /* Process all messages on our queue */
  while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (g_hDlgDepthChange == 0 || !IsDialogMessage (g_hDlgDepthChange, &msg))
	{
	  DispatchMessage (&msg);
	}
    }
  if( strncmp(classname, WINDOW_CLASS_X, strlen(WINDOW_CLASS_X)) == 0 )
       if(UnregisterClass(classname, GetModuleHandle(NULL)) == 0)
            ErrorF("UnregisterClass failed\n");

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("-winDestroyWindowsWindow\n");
#endif
}


/*
 * winUpdateWindowsWindow - Redisplay/redraw a Windows window associated with an X window
 */

static void
winUpdateWindowsWindow (WindowPtr pWin)
{
  winWindowPriv(pWin);
  HWND			hWnd = pWinPriv->hWnd;
  WinXWMHints           hints;

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winUpdateWindowsWindow\n");
#endif

  /* Check if the Windows window's parents have been destroyed */
  if (pWin->parent != NULL
      && pWin->parent->parent == NULL
      && pWin->mapped)
    {
      /* Create the Windows window if it has been destroyed */
      if (hWnd == NULL)
	{
	  winCreateWindowsWindow (pWin);
	  assert (pWinPriv->hWnd != NULL);
	}

      /* Display the window without activating it */
      ShowWindow (pWinPriv->hWnd, SW_SHOWNOACTIVATE);

      /*
       * Minimize window if the initial_state flag is IconicState
       * and if it was not already handled. If the window is created
       * minimized, it is not initialized correctly, so this can be
       * done only after calling the ShowWindow above.
       */

      if (!pWin->overrideRedirect &&
              !pWinPriv -> fIconicHandled &&
                  GetWMHints(pWin, &hints) && (hints.flags & 1L << 1) &&
                      hints.initial_state == 3)
      {
        pWinPriv -> fIconicHandled = TRUE;

        ShowWindow (pWinPriv->hWnd, SW_SHOWMINIMIZED);
      }

      /* Send first paint message */
      UpdateWindow (pWinPriv->hWnd);
    }
  else if (hWnd != NULL)
    {
      /* Destroy the Windows window if its parents are destroyed */
      winDestroyWindowsWindow (pWin);
      assert (pWinPriv->hWnd == NULL);
    }

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("-winUpdateWindowsWindow\n");
#endif
}








typedef struct {
    pointer		value;
    XID			id;
} WindowIDPairRec, *WindowIDPairPtr;





/*
 * winFindWindow -
 */

static void
winFindWindow (pointer value, XID id, pointer cdata)
{
  WindowIDPairPtr	wi = (WindowIDPairPtr)cdata;

  if (value == wi->value)
    {
      wi->id = id;
    }
}


/*
 * winGetWindowID -
 */

static XID
winGetWindowID (WindowPtr pWin)
{
  WindowIDPairRec	wi = {pWin, 0};
  ClientPtr		c = wClient(pWin);

  /* */
  FindClientResourcesByType (c, RT_WINDOW, winFindWindow, &wi);

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winGetWindowID - Window ID: %d\n", wi.id);
#endif

  return wi.id;
}

/*
 *  * winMoveXWindow -
 */

void
winMoveXWindow (WindowPtr pWin, int x, int y)
{
  XID *vlist = malloc(sizeof(long)*2);

  (CARD32*)vlist[0] = x;
  (CARD32*)vlist[1] = y;
  ConfigureWindow (pWin, CWX | CWY, vlist, wClient(pWin));
  free(vlist);
}


/*
 *  * winResizeXWindow -
 */

void
winResizeXWindow (WindowPtr pWin, int w, int h)
{
  XID *vlist = malloc(sizeof(long)*2);

  (CARD32*)vlist[0] = w;
  (CARD32*)vlist[1] = h;
  ConfigureWindow (pWin, CWWidth | CWHeight, vlist, wClient(pWin));
  free(vlist);
}

/*
 * SendConfigureNotify -
 */

static void
SendConfigureNotify(WindowPtr pWin)
{
  xEvent		event;
  winWindowPriv(pWin);

  event.u.u.type = ConfigureNotify;
  event.u.configureNotify.window = pWin->drawable.id;

  if (pWin->nextSib)
    event.u.configureNotify.aboveSibling = pWin->nextSib->drawable.id;
  else
    event.u.configureNotify.aboveSibling = None;

  event.u.configureNotify.x = pWinPriv->iX - wBorderWidth (pWin);
  event.u.configureNotify.y = pWinPriv->iY - wBorderWidth (pWin);

  event.u.configureNotify.width = pWinPriv->iWidth;
  event.u.configureNotify.height = pWinPriv->iHeight;

  event.u.configureNotify.borderWidth = wBorderWidth (pWin);

  event.u.configureNotify.override = pWin->overrideRedirect;

  /* */
  DeliverEvents (pWin, &event, 1, NullWindow);
}

/*
 * Scale an X icon bitmap into a Windoze icon bitmap
 */

static void
winScaleXBitmapToWindows (int iconSize,
			  int effBPP,
			  PixmapPtr pixmap,
			  unsigned char *image)
{
  int			row, column, effXBPP, effXDepth;
  unsigned char		*outPtr;
  unsigned char		*iconData = 0;
  int			stride, xStride;
  float			factX, factY;
  int			posX, posY;
  unsigned char		*ptr;
  unsigned int		zero;
  unsigned int		color;


  if (pixmap->drawable.bitsPerPixel == 15)
    effXBPP = 16;
  else
    effXBPP = BitsPerPixel(pixmap -> drawable.depth);
  
  if (pixmap->drawable.depth == 15)
    effXDepth = 16;
  else
    effXDepth = pixmap->drawable.depth;

  /* Need 32-bit aligned rows */
  stride = ((iconSize * effBPP + 31) & (~31)) / 8;
  xStride  = ((pixmap->drawable.width * effXBPP + 31) & (~31)) / 8;
  
  iconData = malloc (xStride * pixmap->drawable.height);
  miGetImage ((DrawablePtr) &(pixmap->drawable), 0, 0,
	      pixmap->drawable.width, pixmap->drawable.height,
	      ZPixmap, 0xffffffff, iconData);
  
  /* Keep aspect ratio */
  factX = ((float)pixmap->drawable.width) / ((float)iconSize);
  factY = ((float)pixmap->drawable.height) / ((float)iconSize);
  if (factX > factY)
    factY = factX;
  else
    factX = factY;
  
  /* Out-of-bounds, fill icon with zero */
  zero = 0;
 
  for (row = 0; row < iconSize; row++)
    {
      outPtr = image + stride * row;
      for (column = 0; column < iconSize; column++)
	{
	  posX = factX * column;
	  posY = factY * row;
	  
	  ptr = iconData + posY*xStride;
	  if (effXBPP == 1)
	    {
	      ptr += posX / 8;
	      
	      /* Out of X icon bounds, leave space blank */
	      if (posX >= pixmap->drawable.width
		  || posY >= pixmap->drawable.height)
		ptr = (unsigned char *) &zero;
	      
	      if ((*ptr) & (1 << (posX & 7)))
		switch (effBPP)
		  {
		  case 32:
		    *(outPtr++) = 0;
		  case 24:
		    *(outPtr++) = 0;
		  case 16:
		    *(outPtr++) = 0;
		  case 8:
		    *(outPtr++) = 0;
		    break;
		  case 1:
		    outPtr[column / 8] &= ~(1 << (7 - (column & 7)));
		    break;
		  }
	      else
		switch (effBPP)
		  {
		  case 32:
		    *(outPtr++) = 255;
		  case 24:
		    *(outPtr++) = 255;
		  case 16:
		    *(outPtr++) = 255;
		  case 8: 
		    *(outPtr++) = 255;
		    break;
		  case 1:
		    outPtr[column / 8] |= (1 << (7 - (column & 7)));
		    break;
		  }
	    }
	  else if (effXDepth == 24 || effXDepth == 32)
	    {
	      ptr += posX * (effXBPP / 8);

	      /* Out of X icon bounds, leave space blank */
	      if (posX >= pixmap->drawable.width
		  || posY >= pixmap->drawable.height)
		ptr = (unsigned char *) &zero;
	      color = (((*ptr) << 16)
		       + ((*(ptr + 1)) << 8)
		       + ((*(ptr + 2)) << 0));
	      switch (effBPP)
		{
		case 32:
		  *(outPtr++) = *(ptr++); // alpha
		  *(outPtr++) = *(ptr++); // green
		  *(outPtr++) = *(ptr++); // red
		  *(outPtr++) = *(ptr++); // blue
		  break;
		case 24:
		  *(outPtr++) = *(ptr++);
		  *(outPtr++) = *(ptr++);
		  *(outPtr++) = *(ptr++);
		  break;
		case 16:
		  color = ((((*ptr) >> 2) << 10)
			   + (((*(ptr + 1)) >> 2) << 5)
			   + (((*(ptr + 2)) >> 2)));
		  *(outPtr++) = (color >> 8);
		  *(outPtr++) = (color & 255);
		  break;
		case 8:
		  color = (((*ptr))) + (((*(ptr + 1)))) + (((*(ptr + 2))));
		  color /= 3;
		  *(outPtr++) = color;
		  break;
		case 1:
		  if (color)
		    outPtr[column / 8] |= (1 << (7 - (column & 7)));
		  else
		    outPtr[column / 8] &= ~(1 << (7 - (column & 7)));
		}
	    }
	  else if (effXDepth == 16)
	    {
	      ptr += posX * (effXBPP / 8);
	
	      /* Out of X icon bounds, leave space blank */
	      if (posX >= pixmap->drawable.width
		  || posY >= pixmap->drawable.height)
		ptr = (unsigned char *) &zero;
	      color = ((*ptr) << 8) + (*(ptr + 1));
	      switch (effBPP)
		{
		case 32:
		  *(outPtr++) = 0; // alpha
		  *(outPtr++) = (color & 31) << 2;
		  *(outPtr++) = ((color >> 5) & 31) << 2;
		  *(outPtr++) = ((color >> 10) & 31) << 2;
		  break;
		case 24:
		  *(outPtr++) = (color & 31) << 2;
		  *(outPtr++) = ((color >> 5) & 31) << 2;
		  *(outPtr++) = ((color >> 10) & 31) << 2;
		  break;
		case 16:
		  *(outPtr++) = *(ptr++);
		  *(outPtr++) = *(ptr++);
		  break;
		case 8:
		  *(outPtr++) = (((color & 31)
				  + ((color >> 5) & 31)
				  + ((color >> 10) & 31)) / 3) << 2;
		  break;
		case 1:
		  if (color)
		    outPtr[column / 8] |= (1 << (7 - (column & 7)));
		  else
		    outPtr[column / 8] &= ~(1 << (7 - (column & 7)));
		  break;
		} /* end switch(effbpp) */
	    } /* end if effxbpp==16) */
	} /* end for column */
    } /* end for row */
  free (iconData);
}


/*
 * Attempt to create a custom icon from the WM_HINTS bitmaps
 */

static HICON
winXIconToHICON ( PixmapPtr iconPtr, PixmapPtr maskPtr)
{
  unsigned char		*mask, *image, *imageMask;
  unsigned char		*dst, *src;
  //PixmapPtr		iconPtr;
  //PixmapPtr		maskPtr;
  int			iconSize, planes, bpp, effBPP, stride, maskStride, i;
  HDC			hDC;
  ICONINFO		ii;
  HICON			hIcon;

  //GetWMHints (pWin, &hints);
  //if (!hints.icon_pixmap) return NULL;

  //iconPtr = LookupIDByType (hints.icon_pixmap, RT_PIXMAP);
  
  //if (!iconPtr) return NULL;
  
  iconSize = 32;
  
  hDC = GetDC (GetDesktopWindow ());
  planes = GetDeviceCaps (hDC, PLANES);
  bpp = GetDeviceCaps (hDC, BITSPIXEL);
  ReleaseDC (GetDesktopWindow (), hDC);
  
  /* 15 BPP is really 16BPP as far as we care */
  if (bpp == 15)
    effBPP = 16;
  else
    effBPP = bpp;
  
  /* Need 32-bit aligned rows */
  stride = ((iconSize * effBPP + 31) & (~31)) / 8;

  /* Mask is 1-bit deep */
  maskStride = ((iconSize * 1 + 31) & (~31)) / 8; 

  image = (unsigned char * ) malloc (stride * iconSize);
  imageMask = (unsigned char *) malloc (stride * iconSize);
  mask = (unsigned char *) malloc (maskStride * iconSize);
  
  /* Default to a completely black mask */
  memset (mask, 0, maskStride * iconSize);
  
  winScaleXBitmapToWindows (iconSize, effBPP, iconPtr, image);
  //maskPtr = LookupIDByType (hints.icon_mask, RT_PIXMAP);

  if (maskPtr) 
    {
      winScaleXBitmapToWindows (iconSize, 1, maskPtr, mask);
      
      winScaleXBitmapToWindows (iconSize, effBPP, maskPtr, imageMask);
      
      /* Now we need to set all bits of the icon which are not masked */
      /* on to 0 because Color is really an XOR, not an OR function */
      dst = image;
      src = imageMask;

      for (i = 0; i < (stride * iconSize); i++)
	if ((*(src++)))
	  *(dst++) = 0;
	else
	  dst++;
    }
  
  ii.fIcon = TRUE;
  ii.xHotspot = 0; /* ignored */
  ii.yHotspot = 0; /* ignored */
  
  /* Create Win32 mask from pixmap shape */
  ii.hbmMask = CreateBitmap (iconSize, iconSize, planes, 1, mask);

  /* Create Win32 bitmap from pixmap */
  ii.hbmColor = CreateBitmap (iconSize, iconSize, planes, bpp, image);

  /* Merge Win32 mask and bitmap into icon */
  hIcon = CreateIconIndirect (&ii);

  /* Release Win32 mask and bitmap */
  DeleteObject (ii.hbmMask);
  DeleteObject (ii.hbmColor);

  /* Free X mask and bitmap */
  free (mask);
  free (image);
  free (imageMask);

  return hIcon;
}



static HICON 
privLoadIcon(WindowPtr pWin)
{
      Atom retType;
      Atom                  atom = 35; //XA_WM_HINTS;
      int                   retFormat, retValue;
      unsigned long         nItems, bytesLeft, origLen; 
      PixmapPtr             pPtr = NULL, pPtrMask = NULL;
      WinXWMHints           *pHints;
      HICON                 hIcon = 0;

      
      retValue = GetWindowProperty(pWin, atom, 0L, 0L, 0, AnyPropertyType, &retType, &retFormat, 
                            &nItems, &bytesLeft, (unsigned char**)NULL);
      if( retValue == Success ){
          GetWindowProperty(pWin, atom, 0L, bytesLeft, 0, AnyPropertyType, &retType, &retFormat, 
                            &nItems, &origLen, (unsigned char**)&pHints);
	  
	  /* Check if IconPixmapHint is set */
     	  if( pHints->flags & 1L << 2 ){
		pPtr = LookupIDByType (pHints->icon_pixmap, RT_PIXMAP);
	  	/* Check if IconMaskHint is set */
	  	if( pHints->flags & 1L << 5 )
		  pPtrMask = LookupIDByType (pHints->icon_mask, RT_PIXMAP);
	  	if( pPtr )
      			hIcon = winXIconToHICON(pPtr, pPtrMask);
	  }

          /* If no icon is hinted, the default was the icon of a sibling window.
           * Using the server icon seems more suitable.
           *
	   * else{
	   *         WindowPtr sibling = pWin;
           *
	   *         while( sibling && sibling->prevSib )
	   *       	  sibling = sibling->prevSib;
           *
	   *         while( sibling ){
	   *         	winWindowPriv(sibling);
           *
	   *       	if( pWinPriv->hWnd ){
	   *         		hIcon = (HICON)GetClassLong(pWinPriv->hWnd, GCL_HICON);
	   *       		break;
	   *       	}
	   *         	sibling = sibling->nextSib;
	   *         }
           * }
           */

      }	
      if( hIcon == 0 )
	      hIcon = LoadIcon (g_hInstance, MAKEINTRESOURCE(IDI_ICON2));
     return hIcon;
}
	

void
winUpdateWindowName(WindowPtr pWin)
{
  char *name;
  winWindowPriv(pWin);

  if (pWinPriv->hWnd != NULL)
  {
    winGetWindowName(pWin, &name);

    if (name != NULL)
    {
      SetWindowText(pWinPriv->hWnd, name);

      free(name);
    }
  }
}
