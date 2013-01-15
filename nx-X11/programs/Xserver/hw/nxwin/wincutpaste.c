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
 * Authors:	Harold L Hunt II
 */
/* $XFree86: xc/programs/Xserver/hw/xwin/wincutpaste.c,v 1.2 2001/09/07 08:41:54 alanh Exp $ */

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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>


#include "X.h"
#define NEED_EVENTS
#include "Xproto.h"
#include "Xatom.h"
#include "windowstr.h"
#include "selection.h"

#undef MINSHORT
#undef MAXSHORT

#define NONAMELESSUNION
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <windows.h>
#include <windowsx.h>

#undef CreateWindow
#undef FreeResource

#define MaxSelections 2

#define MIN(a,b) ((a) < (b) ? (a) : (b))

extern          WindowPtr *WindowTable;
extern          Selection *CurrentSelections;
extern int      NumCurrentSelections;
static Bool     nxwinSelection = FALSE;
extern Bool     nxwinMultiwindow;

extern int      nxwinUpdateClipboard;

Atom            nxwinLastSelection = 0;

int             nxwinClipboardStatus;

HWND            lastHwnd;


Window          lastOwnerWindow;
WindowPtr       lastOwnerWindowPtr;

Atom            clientTARGETS;
Atom            clientTEXT;
Atom            clientUTF8_STRING;
Atom            clientCutProperty;
Atom            clientCLIPBOARD;

Bool            windowsOwner = FALSE;
Bool            clientOwner  = FALSE;
ClientPtr       lastOwnerClientPtr;

       

extern int GetWindowProperty(WindowPtr, Atom, long, long, 
		Bool, Atom, Atom*, int*, unsigned long*, unsigned long*, unsigned char**);

void nxwinSetWindowClipboard(char *text, int iLength);
void UNIXtoDOS (char **ppszData, int iLength);
void DOStoUNIX (char *pszSrc, int iLength);


void nxwinClearSelection(void)
{
   xEvent       x;
   int          i;
   if (!nxwinClipboardStatus)
     return;

#ifdef NXWIN_CLIPBOARD_DEBUG
   ErrorF("ClearSelection\n");
#endif

for (i = 0; i < MIN(MaxSelections, NumCurrentSelections); i++)
{
   x.u.u.type = SelectionClear;
   x.u.selectionClear.time = GetTimeInMillis();
   x.u.selectionClear.window = CurrentSelections[i].window;
   x.u.selectionClear.atom = CurrentSelections[i].selection;

   if (CurrentSelections[i].client != NULL)
   {
     (void) TryClientEvents (CurrentSelections[i].client , &x, 1,
                             NoEventMask, NoEventMask /* CantBeFiltered */,
                             NullGrab);
   }
   
   if (WindowTable[0])
   {
     CurrentSelections[i].window = WindowTable[0]->drawable.id;
   }

   CurrentSelections[i].client = NullClient;
}

   windowsOwner = TRUE;
   nxwinSelection = FALSE;
}

void nxwinInitSelection(HWND hwnd)
{
   lastHwnd = hwnd;
   clientTARGETS        = MakeAtom("TARGETS", strlen("TARGETS"), TRUE);
   clientTEXT           = MakeAtom("TEXT", strlen("TEXT"), TRUE);
   clientUTF8_STRING    = MakeAtom("UTF8_STRING", strlen("UTF8_STRING"), TRUE);
   clientCutProperty    = MakeAtom("NX_CUT_BUFFER_CLIENT", strlen("NX_CUT_BUFFER_CLIENT"), TRUE);
   clientCLIPBOARD      = MakeAtom("CLIPBOARD", strlen("CLIPBOARD"), TRUE);
   nxwinClipboardStatus = TRUE;
   windowsOwner         = TRUE;
/*   nxwinSetWindowClipboard(" ");*/
}

void nxwinSetSelectionOwner(Selection *pSelection)
{
   if (!nxwinClipboardStatus)
     return;

#ifdef NXWIN_CLIPBOARD_DEBUG
   ErrorF("SetSelectionOwner\n");
#endif

   lastOwnerWindowPtr = pSelection->pWin;
   lastOwnerWindow    = pSelection->window;
   lastOwnerClientPtr = pSelection->client;
   
   nxwinSelection = TRUE;

   if (OpenClipboard(lastHwnd))
   {
     windowsOwner = FALSE;
     EmptyClipboard();
     CloseClipboard();
   }

   nxwinUpdateClipboard = 1;
   nxwinLastSelection = pSelection->selection;
/*
    if (pSelection->selection == XA_PRIMARY)
    {
       xEvent x;

       lastOwnerWindowPtr = pSelection->pWin;
       lastOwnerWindow    = pSelection->window;

       x.u.u.type = SelectionRequest;
       x.u.selectionRequest.time = GetTimeInMillis();
       x.u.selectionRequest.owner = pSelection->window;
       x.u.selectionRequest.requestor = WindowTable[0]->drawable.id;
       x.u.selectionRequest.selection = pSelection->selection;
       x.u.selectionRequest.target = XA_STRING;
       x.u.selectionRequest.property = clientCutProperty;

       (void) TryClientEvents (pSelection->client, &x, 1,
                                NoEventMask, NoEventMask ,
                                NullGrab);
#ifdef NXWIN_CLIPBOARD_DEBUG
       ErrorF("SetSelectionOwner XA_PRIMARY \n");
#endif

       windowsOwner = FALSE;   
     }
*/
}

Bool nxwinConvertSelection(ClientPtr client, WindowPtr pWin, Atom selection,
                               Window requestor, Atom property, Atom target, Time time)
{
   if (!nxwinClipboardStatus)
     return 0;

   #ifdef NXWIN_CLIPBOARD_DEBUG
   ErrorF("ConvertSelection\n");
   #endif

   if (!windowsOwner) /* there is a X client owner, let normal stuff happens */
       return 0;

   if (target == clientTARGETS)
   {
      Atom xa_STRING[2];
      xEvent x;

      xa_STRING[0] = XA_STRING;
      xa_STRING[1] = clientUTF8_STRING;

      ChangeWindowProperty(pWin,
                           property,
                           target,
                           sizeof(Atom)*8,
                           PropModeReplace,
                           2,
                           &xa_STRING, 1);

      x.u.u.type = SelectionNotify;
      x.u.selectionNotify.time = time;
      x.u.selectionNotify.requestor = requestor;
      x.u.selectionNotify.selection = selection;
      x.u.selectionNotify.target = target;
      x.u.selectionNotify.property = property;
      (void) TryClientEvents(client, &x, 1, NoEventMask, NoEventMask , NullGrab);
      return 1;
   }

   #ifdef NXWIN_CLIPBOARD_DEBUG
   ErrorF("ConvertSelection converting...\n");
   #endif

   if ((target == clientTEXT) ||
           (target == XA_STRING) ||
               (target == clientUTF8_STRING))
   {
      HGLOBAL hGlobal;
      char *pszGlobalData;
      xEvent x;

      /* Access the clipboard */
      if (!OpenClipboard (lastHwnd)) return 0;

      if (target == clientUTF8_STRING)
      {
        hGlobal = GetClipboardData (CF_UNICODETEXT);
      }
      else
      {
        hGlobal = GetClipboardData (CF_TEXT);
      }

      if (!hGlobal)
      {
        CloseClipboard();
        x.u.u.type = SelectionNotify;
        x.u.selectionNotify.time = time;
        x.u.selectionNotify.requestor = requestor;
        x.u.selectionNotify.selection = selection;
        x.u.selectionNotify.target = target;
        x.u.selectionNotify.property = None;
        (void) TryClientEvents(client, &x, 1, NoEventMask,
                               NoEventMask , NullGrab);
        return 1;
      }
      pszGlobalData = (char *) GlobalLock (hGlobal);

      #ifdef NXWIN_CLIPBOARD_DEBUG
      ErrorF("Converting selection for client [%p] with window [%p] "
                 "target [%d] clipboard data [%p].\n", client, pWin,
                     target, pszGlobalData);
      #endif

      if (target == clientUTF8_STRING)
      {
        int bytesNeeded;
        HGLOBAL hGlobal2;
        char *pszGlobalData2 = NULL;

        bytesNeeded = WideCharToMultiByte(CP_UTF8, 0, (WCHAR *) pszGlobalData, -1,
                                              pszGlobalData2, 0, NULL, NULL) + 1;
  
        hGlobal2 = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, bytesNeeded);

        if (hGlobal2 == NULL)
        {
          ErrorF("nxwinConvertSelection: WARNING! Memory allocation failed with error %d.\n",
                     GetLastError());
        }

        pszGlobalData2 = GlobalLock(hGlobal2);
  

        #ifdef NXWIN_CLIPBOARD_DEBUG
        ErrorF("UTF8 conversion needs [%d] bytes allocated on [%p] and locked on [%p].\n",
                   bytesNeeded, hGlobal2, pszGlobalData2);
        #endif

        /* Convert Unicode text to UTF8 */
        WideCharToMultiByte(CP_UTF8, 0, (WCHAR *) pszGlobalData, -1, pszGlobalData2,
                                bytesNeeded, NULL, NULL);

        if (pszGlobalData2 == NULL)
        {
          ErrorF("nxwinConvertSelection: WARNING! Unicode conversion failed with error %d.\n",
                     GetLastError());

          pszGlobalData2 = pszGlobalData;

          GlobalUnlock(hGlobal2);
          GlobalFree(hGlobal2);

          hGlobal2 = NULL;
        }

        #ifdef NXWIN_CLIPBOARD_DEBUG
        ErrorF("Converted UTF8 string in [%p].\n", pszGlobalData2);
        #endif

        #ifdef NXWIN_CLIPBOARD_DEBUG
        ErrorF("Converting selection string from DOS to UNIX\n");
        #endif

        /* Convert DOS string to UNIX string */
        DOStoUNIX (pszGlobalData2, strlen (pszGlobalData2));

        /* Copy the clipboard text to the requesting window */
        ChangeWindowProperty(pWin,
                             property,
                             target,
                             8,
                             PropModeReplace,
                             strlen(pszGlobalData2),
                             pszGlobalData2, 1);

        if (hGlobal2 != NULL)
        {
          GlobalUnlock(hGlobal2);
          GlobalFree(hGlobal2);
        }
      }
      else if (pszGlobalData != NULL)
      {
        #ifdef NXWIN_CLIPBOARD_DEBUG
        ErrorF("Converting selection string from DOS to UNIX\n");
        #endif

        /* Convert DOS string to UNIX string */
        DOStoUNIX (pszGlobalData, strlen (pszGlobalData));

        /* Copy the clipboard text to the requesting window */
        ChangeWindowProperty(pWin,
                             property,
                             target,
                             8,
                             PropModeReplace,
                             strlen(pszGlobalData),
                             pszGlobalData, 1);
      }

      /* Release the clipboard data */
      GlobalUnlock (hGlobal);
      pszGlobalData = NULL;
      CloseClipboard ();

      x.u.u.type = SelectionNotify;
      x.u.selectionNotify.time = time;
      x.u.selectionNotify.requestor = requestor;
      x.u.selectionNotify.selection = selection;
      x.u.selectionNotify.target = target;
      x.u.selectionNotify.property = property;
      (void) TryClientEvents(client, &x, 1, NoEventMask,
                             NoEventMask, NullGrab);
      return 1;
   }

   return 0;
}

/*
 * Change \n to \r\n.  Reallocate string if necessary.
 */

void
UNIXtoDOS (char **ppszData, int iLength)
{
  int	    iNewlineCount = 0;
  char      *pszSrc = *ppszData;
  char      *pszEnd = pszSrc + iLength;
  char      *pszDest = NULL, *pszDestBegin = NULL;

  /* Count \n characters without leading \r */
  while (pszSrc < pszEnd)
  {
  /* Skip ahead two character if found set of \r\n */
  if (*pszSrc == '\n')
  {
     iNewlineCount++;
     if (pszSrc > *ppszData)
     {
        iNewlineCount += (*(pszSrc - 1 ) == '\r')?-1:0;
     }
   }
    pszSrc++;
  }

  /* Return if no naked \n's */
  if (iNewlineCount == 0)
     return;

  /* Allocate a new string */
  pszDestBegin = pszDest = xalloc (iLength + iNewlineCount + 1);

  /* Set source pointer to beginning of data string */
  pszSrc = *ppszData;

  /* Loop through all characters in source string */
  while (pszSrc < pszEnd)
  {
      /* Copy line endings that are already valid */
    if (*pszSrc == '\r')
    {
       if (pszSrc < pszEnd)
         if (*(pszSrc + 1 ) == '\n')
         {
            *pszDest++ = *pszSrc++;
            *pszDest++ = *pszSrc++;
            continue;
         }
    }

    /* Add \r to naked \n's */
    if (*pszSrc == '\n')
    {
       *pszDest++ = '\r';
       *pszDest++ = *pszSrc++;
       continue;
    }

    /* Copy normal characters */
    *pszDest++ = *pszSrc++;
  }

  /* Put terminating null at end of new string */
  *pszDest = '\0';

  /* Swap string pointers */

  xfree (*ppszData);
  *ppszData = pszDestBegin;

}


void
DOStoUNIX (char *pszSrc, int iLength)
{
  char  *pszDest = pszSrc;
  char  *pszEnd = pszSrc + iLength;

  /* Loop until the last character */
  while (pszSrc < pszEnd)
  {
    /* Copy the current source character to current destination character */
    *pszDest = *pszSrc;

    /* Advance to the next source character */
    pszSrc++;

    /* Don't advance the destination character if we need to drop an \r */
    if (*pszDest != '\r' || *pszSrc != '\n')
        pszDest++;
   }

  /* Move the terminating null */
  *pszDest = '\0';
}

void nxwinSetWindowClipboard(char *text, int iLength)
{

  HGLOBAL       hGlobal;
  char          *pszGlobalData;
  char          *pszTemp;

  if (!nxwinClipboardStatus)
     return;

  if (OpenClipboard(lastHwnd))
  {
     /* allocate memory that can be free at end */

     pszTemp = xalloc(iLength + 1);

     if (pszTemp == NULL)
     {
        CloseClipboard();
        return;
     }

      memcpy(pszTemp, text, iLength);
      *(pszTemp + iLength) = 0;

      UNIXtoDOS(&pszTemp, iLength);

      hGlobal = GlobalAlloc (GMEM_MOVEABLE, strlen(pszTemp) + 1);

      /* Obtain a pointer to the global memory */

      pszGlobalData = GlobalLock (hGlobal);

      if (pszGlobalData == NULL)
      {
        CloseClipboard();
        GlobalFree(hGlobal);
        return;
      }

      /* Copy the string into the global memory */
      strcpy(pszGlobalData, pszTemp);

      /* free the allocated memory */
      xfree(pszTemp);

      /* Release the pointer to the global memory */
      GlobalUnlock (hGlobal);
      pszGlobalData = NULL;

      if (windowsOwner)
      {
        windowsOwner = FALSE;
        EmptyClipboard();
      }
      /* Push the selection data to the Windows clipboard */
      SetClipboardData (CF_TEXT, hGlobal);
      CloseClipboard();
  }
}


WindowPtr nxwinTryValidWindow(Atom property, WindowPtr pWin)
{
  return (((property == clientCutProperty) && (lastOwnerWindowPtr))?lastOwnerWindowPtr:pWin);
}

Bool nxwinSendNotify(xEvent* x)
{
 int iReturn;

#ifdef NXWIN_CLIPBOARD_DEBUG
  ErrorF("SendNotify\n");
#endif
  if (x->u.selectionNotify.property == clientCutProperty)
  {
     Atom       atomReturnType;
     int     iReturnFormat;
     unsigned long   ulReturnItems;
     unsigned long   ulReturnBytesLeft;
     unsigned char   *pszReturnData = NULL;
     iReturn = GetWindowProperty(lastOwnerWindowPtr, clientCutProperty, 0, 0, FALSE,
                                AnyPropertyType, &atomReturnType, &iReturnFormat,
                                &ulReturnItems, &ulReturnBytesLeft, &pszReturnData);

     if ((iReturn == Success) && (ulReturnBytesLeft > 0))
     {
        iReturn = GetWindowProperty(lastOwnerWindowPtr,clientCutProperty,0,ulReturnBytesLeft, FALSE,
                                        AnyPropertyType, &atomReturnType, &iReturnFormat,
                                        &ulReturnItems, &ulReturnBytesLeft, &pszReturnData);
        if ((iReturn == Success) && (ulReturnItems > 0))
        {
          nxwinSetWindowClipboard(pszReturnData, ulReturnItems);

          clientOwner = TRUE;
          /* nxwinClearSelection(); */

          return TRUE;
        }
     }
  }
  return FALSE;
}

void nxwinRequestSelection(void)
{
  if (lastOwnerWindowPtr)
  {
    xEvent x;

    x.u.u.type = SelectionRequest;
    x.u.selectionRequest.time = GetTimeInMillis();
    x.u.selectionRequest.owner = lastOwnerWindow;
    x.u.selectionRequest.requestor = WindowTable[0]->drawable.id;
    x.u.selectionRequest.selection = nxwinLastSelection;
    x.u.selectionRequest.target = XA_STRING;
    x.u.selectionRequest.property = clientCutProperty;

    (void) TryClientEvents (lastOwnerClientPtr, &x, 1,
                                NoEventMask, NoEventMask /* CantBeFiltered */,
                                NullGrab);
    SetCriticalOutputPending();
  }
}

void nxwinGotFocus(wParam)
{
  if (nxwinMultiwindow == 0 || wParam == 0 || nxwinSelection == 0)
  {
    nxwinClearSelection();
    SetCriticalOutputPending();
  }
}

void nxwinLostFocus(void)
{
#if 0
   if (lastOwnerWindowPtr)
   {
      xEvent x;

      x.u.u.type = SelectionRequest;
      x.u.selectionRequest.time = GetTimeInMillis();
      x.u.selectionRequest.owner = lastOwnerWindow;
      x.u.selectionRequest.requestor = WindowTable[0]->drawable.id;
      x.u.selectionRequest.selection = XA_PRIMARY;
      x.u.selectionRequest.target = XA_STRING;
      x.u.selectionRequest.property = clientCutProperty;

      (void) TryClientEvents (lastOwnerClientPtr, &x, 1,
                               NoEventMask, NoEventMask /* CantBeFiltered */,
                               NullGrab);
      SetCriticalOutputPending();
   }
#endif
}

WindowPtr FindSelectionOwner(Atom selection)
{
  int i = 0;

  while ((i < NumCurrentSelections) &&
         CurrentSelections[i].selection != selection) i++;
                                                                                                  
  if (i < NumCurrentSelections)
  {
    return CurrentSelections[i].pWin;
  }

  return 0;  
}

