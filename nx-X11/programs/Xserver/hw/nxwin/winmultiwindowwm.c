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
/* $XFree86: xc/programs/Xserver/hw/xwin/winmultiwindowwm.c,v 1.1 2003/02/12 15:01:38 alanh Exp $ */

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

/* X headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
/*
#include <setjmp.h>
*/
#include <pthread.h>
#include <errno.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xlocale.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>

/* Fixups to prevent collisions between Windows and X headers */
#define ATOM DWORD

/* Windows headers */
#include <windows.h>

/* Local headers */
#include "winmultstack.h"
#include "winwindow.h"
#include "../../include/resource.h"
#include "../../include/pixmapstr.h"

extern void **clients;

/*
 * Constant defines
 */

#define WIN_CONNECT_RETRIES	5
#define WIN_CONNECT_DELAY	5
#define WIN_MSG_QUEUE_FNAME	"/dev/windows"
#define	WM_WM_X_EVENT           1
#define WIN_JMP_OKAY		0
#define WIN_JMP_ERROR_IO	2

int nxwinSetInputFocusFlag = 0;
int nxwinSetInputFocusClient = 0;
Window nxwinSetInputFocusFocus = 0;

/*
 * Local structures
 */

typedef struct _WMMsgNodeRec {
  winWMMessageRec	msg;
  struct _WMMsgNodeRec	*pNext;
} WMMsgNodeRec, *WMMsgNodePtr;

typedef struct _WMMsgQueueRec {
  struct _WMMsgNodeRec	*pHead;
  struct _WMMsgNodeRec	*pTail;
  pthread_mutex_t	pmMutex;
  pthread_cond_t	pcNotEmpty;
} WMMsgQueueRec, *WMMsgQueuePtr;

typedef struct _WMInfo {
/*
  Display		*pDisplay;
 */
  WMMsgQueueRec		wmMsgQueue;
  Atom			atmWmProtos;
  Atom			atmWmDelete;
} WMInfoRec, *WMInfoPtr;

typedef struct _WMProcArgRec {
  DWORD			dwScreen;
  WMInfoPtr		pWMInfo;
  pthread_mutex_t	*ppmServerStarted;
} WMProcArgRec, *WMProcArgPtr;

/*
 * References to external symbols
 */

extern char *display;
extern void ErrorF (const char* /*f*/, ...);
extern void FatalError(const char*, ...);
extern Bool g_fCalledSetLocale;
extern int GetWindowProperty(void*, Atom, long, long, Bool, Atom, Atom*, int*, unsigned long*, unsigned long*, unsigned char**);
extern Atom MakeAtom(char*, unsigned, Bool);
extern int ConfigureWindow(void*, Mask, XID*, void*);
extern void* winGetClientPriv(void*);
extern unsigned winGetOverrideRedirectPriv(void*);
extern int UnmapWindow(void*, Bool);
extern int SetInputFocus(void*, int, Window, CARD8, Time, Bool);
extern int winGetinputInfokeyboard();
extern int NXSendClientEventToWindow(void*, Window, Atom, long);
extern void CloseDownClient(void*);	

#ifdef NXWIN_MULTIWINDOW
extern Bool nxwinMultiwindow;
extern pthread_mutex_t nxwinMultiwindowMutex;
extern pthread_mutex_t nxwinMultStackMutex;

MultStackQueuePtr pMultStackQueue;
#endif

/*
 * Prototypes for local functions
 */

static void
PushMessage (WMMsgQueuePtr pQueue, WMMsgNodePtr pNode);

static WMMsgNodePtr
PopMessage (WMMsgQueuePtr pQueue);

static Bool
InitQueue (WMMsgQueuePtr pQueue);

static void
/* FIXME: pWin is a WindowPtr */
GetWindowName (void *pWin, char **ppName);

static void*
winMultiWindowWMProc (void* pArg);

static void
winInitMultiWindowWM (WMInfoPtr pWMInfo, WMProcArgPtr pProcArg);


/*
 * Local globals
 */

static int			g_nQueueSize;
/*
static jmp_buf			g_jmpEntry;
*/

static char nxwinWM_PROTOCOLS[]="WM_PROTOCOLS";
static char nxwinWM_DELETE[] = "WM_DELETE_WINDOW";
/*
 * PushMessage - Push a message onto the queue
 */

static void
PushMessage (WMMsgQueuePtr pQueue, WMMsgNodePtr pNode)
{

  /* Lock the queue mutex */
  pthread_mutex_lock (&pQueue->pmMutex);

  pNode->pNext = NULL;
  
  if (pQueue->pTail != NULL)
    {
      pQueue->pTail->pNext = pNode;
    }
  pQueue->pTail = pNode;
  
  if (pQueue->pHead == NULL)
    {
      pQueue->pHead = pNode;
    }


#if 0
  switch (pNode->msg.msg)
    {
    case WM_WM_MOVE:
      ErrorF ("\tWM_WM_MOVE\n");
      break;
    case WM_WM_RAISE:
      ErrorF ("\tWM_WM_RAISE\n");
      break;
    case WM_WM_LOWER:
      ErrorF ("\tWM_WM_RAISE\n");
      break;
    case WM_WM_MAP:
      ErrorF ("\tWM_WM_MAP\n");
      break;
    case WM_WM_UNMAP:
      ErrorF ("\tWM_WM_UNMAP\n");
      break;
    case WM_WM_KILL:
      ErrorF ("\tWM_WM_KILL\n");
      break;
    default:
      ErrorF ("Unknown Message.\n");
      break;
    }
#endif

  /* Increase the count of elements in the queue by one */
  ++g_nQueueSize;

  /* Release the queue mutex */
  pthread_mutex_unlock (&pQueue->pmMutex);

  /* Signal that the queue is not empty */
  pthread_cond_signal (&pQueue->pcNotEmpty);
}


#if CYGMULTIWINDOW_DEBUG
/*
 * QueueSize - Return the size of the queue
 */

static int
QueueSize (WMMsgQueuePtr pQueue)
{
  WMMsgNodePtr		pNode;
  int			nSize = 0;
  
  /* Loop through all elements in the queue */
  for (pNode = pQueue->pHead; pNode != NULL; pNode = pNode->pNext)
    ++nSize;

  return nSize;
}
#endif


/*
 * PopMessage - 
 */

static WMMsgNodePtr
PopMessage (WMMsgQueuePtr pQueue)
{
  WMMsgNodePtr		pNode;

  /* Lock the queue mutex */
#ifdef NXWIN_MULTIWINDOW_DEBUG
  ErrorF("PopMessage: --> pthread_mutex_lock(pQueue->mutex)\n");
#endif
  pthread_mutex_lock (&pQueue->pmMutex);
#ifdef NXWIN_MULTIWINDOW_DEBUG
  ErrorF("PopMessage: <-- pthread_mutex_lock(pQueue->mutex)\n");
#endif

  /* Wait for --- */
  while (pQueue->pHead == NULL)
    {
#ifdef NXWIN_MULTIWINDOW_DEBUG
      ErrorF("Popmessage: --> pthread_cond_wait()\n");	    
#endif
      pthread_cond_wait (&pQueue->pcNotEmpty, &pQueue->pmMutex);
#ifdef NXWIN_MULTIWINDOW_DEBUG
      ErrorF("Popmessage: <-- pthread_cond_wait()\n");	    
#endif
    }
  
  pNode = pQueue->pHead;
  if (pQueue->pHead != NULL)
    {
      pQueue->pHead = pQueue->pHead->pNext;
    }

  if (pQueue->pTail == pNode)
    {
      pQueue->pTail = NULL;
    }

  /* Drop the number of elements in the queue by one */
  --g_nQueueSize;

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("Queue Size %d %d\n", g_nQueueSize, QueueSize(pQueue));
#endif
  
  /* Release the queue mutex */
#ifdef NXWIN_MULTIWINDOW_DEBUG
  ErrorF("PopMessage: --> pthread_mutex_unlock(pQueue->mutex)\n");
#endif
  pthread_mutex_unlock (&pQueue->pmMutex);
#ifdef NXWIN_MULTIWINDOW_DEBUG
  ErrorF("PopMessage: --> pthread_mutex_unlock(pQueue->mutex)\n");
#endif

  return pNode;
}


#if 0
/*
 * HaveMessage - 
 */

static Bool
HaveMessage (WMMsgQueuePtr pQueue, UINT msg, Window iWindow)
{
  WMMsgNodePtr pNode;
  
  for (pNode = pQueue->pHead; pNode != NULL; pNode = pNode->pNext)
    {
      if (pNode->msg.msg==msg && pNode->msg.iWindow==iWindow)
	return True;
    }
  
  return False;
}
#endif


/*
 * InitQueue - Initialize the Window Manager message queue
 */

static
Bool
InitQueue (WMMsgQueuePtr pQueue)
{
  /* Check if the pQueue pointer is NULL */
  if (pQueue == NULL)
    {
      ErrorF ("InitQueue: - pQueue is NULL.  Exiting.\n");
      return FALSE;
    }

  /* Set the head and tail to NULL */
  pQueue->pHead = NULL;
  pQueue->pTail = NULL;

  /* There are no elements initially */
  g_nQueueSize = 0;

#if CYGMULTIWINDOW_DEBUG
  ErrorF ("InitQueue: - Queue Size %d %d\n", g_nQueueSize, QueueSize(pQueue));
#endif

  ErrorF ("InitQueue: - Calling pthread_mutex_init\n");

  /* Create synchronization objects */
  pthread_mutex_init (&pQueue->pmMutex, NULL);

  ErrorF ("InitQueue: pthread_mutex_init returned\n");
  ErrorF ("InitQueue: calling pthread_cond_init\n");
  
  pthread_cond_init (&pQueue->pcNotEmpty, NULL);

  ErrorF ("InitQueue: pthread_cond_init returned\n");
  
  return TRUE;
}


/*
 * GetWindowName - 
 */
static int 
privateGetWindowName(void *pWin, char **ppName, Atom atom)
{  
   int                   retFormat, retValue;
   unsigned long         nItems, bytesLeft, origLen;
   unsigned char         *strName;
   Atom                  retType;
   	
   /* FIXME: look for memory leak */
   if( (retValue = GetWindowProperty(pWin, atom, 0L, 0L, False,
                        AnyPropertyType, &retType, &retFormat, 
                        &nItems, &bytesLeft, (unsigned char**)NULL)) != Success){
       ErrorF("GetWindowName: GetWindowProperty failed\n");
       return retValue; 
   }
   origLen = bytesLeft;
   if( !( strName = malloc( bytesLeft + 1) ) )
       FatalError("GetWindowName: malloc failed\n");	   
   if( (retValue = GetWindowProperty(pWin, atom, 0L, bytesLeft, False,
                        AnyPropertyType, &retType, &retFormat, 
                        &nItems, &bytesLeft, (unsigned char**)ppName)) != Success){
       ErrorF("GetWindowName: GetWindowProperty failed\n");
       free(strName);
       return retValue;
   }
   memcpy(strName, *ppName, origLen);
   strName[origLen] = 0;
   *ppName = strName;
#ifdef NXWIN_MULTIWINDOW_DEBUG
   ErrorF("GetWindowName: strName %s\n\torigLen %d\n\t*ppName %s\n", strName, origLen, *ppName);
#endif
   return Success;
}

static void
GetWindowName (void *pWin, char **ppName)
{
   Atom                  reqAtom;
   char                  atom_NET_WM_NAME[] = "_NET_WM_NAME";

   /*
   XTextProperty		xtpName;
   */
#if CYGMULTIWINDOW_DEBUG
   ErrorF ("GetWindowName\n");
#endif

   /* Intialize ppName to NULL */
   *ppName = NULL;

   /* TRY with ATOM WM_NAME */
   reqAtom = XA_WM_NAME;
   if( privateGetWindowName(pWin, ppName, reqAtom) == Success )
	   return;
   reqAtom = MakeAtom(atom_NET_WM_NAME, sizeof(atom_NET_WM_NAME) - 1, True);
   privateGetWindowName(pWin, ppName, reqAtom);
}

/*
 * winMultiWindowWMProc
 */
int
winMultStackWindow(pWin, val)
	/* Should Be WindowPtr */
	void* pWin;
        unsigned long val;          /* needed for macro below */
{

#ifdef NXWIN_MULTIWINDOW
  MultStackNodePtr pMultStackNode;
  MultStackNodePtr prevNode;

#ifdef NXWIN_MULTIWINDOW_DEBUG
  if(nxwinMultiwindow)
       ErrorF("winMultStackWindow: LOCK before queue access\n");
  else
       ErrorF("winMultStackWindow: before ConfigureWindow\n");		
#endif
  if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultStackMutex))
       ErrorF("winMultStackWindow: !!! pthread_mutex_lock failed\n");

  if(nxwinMultiwindow)
  {
    pMultStackNode = (MultStackNodePtr) malloc(sizeof(MultStackNodeRec));
    pMultStackNode -> pWin = pWin;
    pMultStackNode -> val = val;
    pMultStackNode -> pNext = NULL;

    prevNode = pMultStackQueue -> pHead;

    if (prevNode == NULL) {
      pMultStackQueue -> pHead = pMultStackNode;
      pMultStackQueue -> pTail = pMultStackNode;
    }
    else {
      int count = 1;
      while (prevNode -> pNext != NULL) {
        prevNode = prevNode -> pNext;
        count++;
      }
      prevNode -> pNext = pMultStackNode;
      pMultStackQueue -> pTail = pMultStackNode;
    }
  }

  else {
#endif

    if (clients[CLIENT_ID(((DrawableRec*) pWin)->id)])
    {
      if( ConfigureWindow(pWin, CWStackMode, &val, winGetClientPriv(pWin)) != Success)
          ErrorF("winMultStackWindow: ConfigureWindow not returned 0\n");
    }
    else
    {
      ErrorF("winMultStackWindow: ConfigureWindow not called\n");
    }

#ifdef NXWIN_MULTIWINDOW
  }
#ifdef NXWIN_MULTIWINDOW_DEBUG
  if(nxwinMultiwindow)
       ErrorF("winMultStackWindow: UNLOCK after queue access\n");
  else
       ErrorF("winMultStackWindow: after ConfigureWindow\n");
#endif
  if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultStackMutex))
       ErrorF("winMultStackWindow: !!! pthread_mutex_unlock failed\n");
#endif
    /*
    LockDisplay(dpy);
    GetReqExtra(ConfigureWindow, 4, req);
    req->window = w;
    req->mask = CWStackMode;
    OneDataCard32 (dpy, NEXTPTR(req,xConfigureWindowReq), val);
    UnlockDisplay(dpy);
    SyncHandle();
    */
    return 1;
}

static void *
winMultiWindowWMProc (void *pArg)
{
  WMProcArgPtr		pProcArg = (WMProcArgPtr)pArg;
  WMInfoPtr		pWMInfo = pProcArg->pWMInfo;

  pMultStackQueue = (MultStackQueuePtr) malloc(sizeof(MultStackQueueRec));
  pMultStackQueue -> pHead = NULL;
  pMultStackQueue -> pTail = NULL;
 
  /* Initialize the Window Manager */
  winInitMultiWindowWM (pWMInfo, pProcArg);
  
#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winMultiWindowWMProc ()\n");
#endif

  /* Loop until we explicity break out */
  for (;;)
    {
      WMMsgNodePtr	pNode;

      /* Pop a message off of our queue */
#ifdef NXWIN_MULTIWINDOW_DEBUG
      ErrorF("winMultiWindowWMProc: before popMessage\n");
#endif
      pNode = PopMessage (&pWMInfo->wmMsgQueue);
      if (pNode == NULL)
	{
	  /* Bail if PopMessage returns without a message */
	  /* NOTE: Remember that PopMessage is a blocking function. */
	  ErrorF("winMultiWindowWMProc:<<pthread_exit>>\n");
	  pthread_exit (NULL);
	}

#if CYGMULTIWINDOW_DEBUG || defined(NXWIN_MULTIWINDOW_DEBUG)
       ErrorF ("winMultiWindowWMProc: - %d ms MSG: %d ID: %d\n",
	      GetTickCount (), (int)pNode->msg.msg, (int)pNode->msg.dwID);
#endif
      /* Branch on the message type */
      switch (pNode->msg.msg)
	{
#if 0
	case WM_WM_MOVE:
	  ErrorF ("\tWM_WM_MOVE\n");
	  break;

	case WM_WM_SIZE:
	  ErrorF ("\tWM_WM_SIZE\n");
	  break;
#endif

	case WM_WM_RAISE:
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\tWM_WM_RAISE\n");
#endif

	  /* Raise the window */
	  winMultStackWindow(pNode->msg.pWin, Above);
	  /*
	  XRaiseWindow (pWMInfo->pDisplay, pNode->msg.iWindow);
	  */
	  break;

	case WM_WM_LOWER:
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\tWM_WM_LOWER\n");
#endif

	  /* Lower the window */
	  winMultStackWindow(pNode->msg.pWin, Below);
	  /*
	  XLowerWindow (pWMInfo->pDisplay, pNode->msg.iWindow);
	  */
	  break;

	case WM_WM_MAP:
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\tWM_WM_MAP\n");
#endif
	  {
	    /*XWindowAttributes		attr;*/
	    char			*pszName;
#if 0
	    XWMHints			*pHints;
#endif

	    /* Get the window attributes */
	    /*
	    XGetWindowAttributes (pWMInfo->pDisplay,
				  pNode->msg.iWindow,
				  &attr);
             */
	    if (!winGetOverrideRedirectPriv(pNode->msg.pWin))
	      {
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
		if(nxwinMultiwindow)
	             ErrorF("winMultiWindowWMProc: LOCK before GetWindowName\n");
		else
		     ErrorF("winMultiWindowWMProc: before GetWindowName\n");	
#endif
                if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultiwindowMutex))
	                     ErrorF("winMultiWindowWMProc: pthread_mutex_lock failed\n");
#endif
		/* Set the Windows window name */
		GetWindowName(pNode->msg.pWin, &pszName);
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
		if(nxwinMultiwindow)
	             ErrorF("winMultiWindowWMProc: UNLOCK after GetWindowName\n");
		else
		     ErrorF("winMultiWindowWMProc: after GetWindowName\n");
#endif
                if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultiwindowMutex) != 0)
	             ErrorF("winMultiWindowWMProc: !!! pthread_mutex_unlock failed\n");
#endif
		if(!pszName){
		     ErrorF("winMultiWindowWMProc: GetWindowName failed\n");	
		     break;
		}
		SetWindowText (pNode->msg.hwndWindow, pszName);
#ifdef NXWIN_MULTIWINDOW_DEBUG
		ErrorF("winMultiWindowWMProc: Insert here LoadIcon\n");
#endif
		free (pszName);
	      }
	  }
	  break;

	case WM_WM_UNMAP:
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\tWM_WM_UNMAP\n");
#endif
	  
	  /* Unmap the window */
	  /*
	  XUnmapWindow(pWMInfo->pDisplay, pNode->msg.iWindow);
	  */
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	  if(nxwinMultiwindow)
	       ErrorF("winMultiWindowWMProc: LOCK before UnmapWindow\n");
	  else
	       ErrorF("winMultiWindowWMProc: before UnmapWindow\n");
#endif
          if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultiwindowMutex))
               ErrorF("winMultiWindowWMProc: !!! pthread_mutex_lock failed\n");
#endif
	  UnmapWindow(pNode->msg.pWin, False);
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	  if(nxwinMultiwindow)
	       ErrorF("winMultiWindowWMProc: UNLOCK after UnmapWindow\n");
	  else
	       ErrorF("winMultiWindowWMProc: after UnmapWindow\n");
#endif
          if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultiwindowMutex))
               ErrorF("winMultiWindowWMProc: !!! pthread_mutex_unlock failed\n");
#endif
	  break;

	case WM_WM_KILL:
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\tWM_WM_KILL\n");
#endif
#ifdef NXWIN_MULTIWINDOW_DEBUG
	  ErrorF ("winMultiWindowWMProc: WM_WM_KILL\n");
#endif
	  {
	    unsigned long		i, n, found = 0;
	    Atom			*protocols, *my_protocols;
	    Atom                        prop;
            int                         retFormat;
            unsigned long               bytesLeft;
            Atom                        retType;
	    int                         ret;
	    
	  
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	    if(nxwinMultiwindow)
	         ErrorF("winMultiWindowWMProc: LOCK before MakeAtom(%s)\n", nxwinWM_PROTOCOLS);
	    else
	         ErrorF("winMultiWindowWMProc: before MakeAtom(%s)\n", nxwinWM_PROTOCOLS);
#endif
            if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultiwindowMutex))
	         ErrorF("winMultiWindowWMProc: !!! pthread_mutex_lock failed\n");
#endif
	    prop = MakeAtom(nxwinWM_PROTOCOLS, strlen(nxwinWM_PROTOCOLS), True);
	          
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	    if(nxwinMultiwindow)
	         ErrorF("winMultiWindowWMProc: UNLOCK after MakeAtom(%s)\n", nxwinWM_PROTOCOLS);
	    else
	         ErrorF("winMultiWindowWMProc: after MakeAtom(%s)\n", nxwinWM_PROTOCOLS);
#endif
	    if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultiwindowMutex))
		 ErrorF("winMultiWindowWMProc: !!! pthread_mutex_lock failed\n");
#endif
	    /*
	    if (XGetWMProtocols (pWMInfo->pDisplay,
				 pNode->msg.iWindow,
				 &protocols,
				 &n))
	    */
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	    ErrorF("winMultiWindowWMProc: LOCK before GetWindowProperty()\n");
#endif
	    if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultiwindowMutex))
	         ErrorF("winMultiWindowWMProc: !!! pthread_mutex_lock failed\n");
#endif
            ret = GetWindowProperty(pNode->msg.pWin, prop, 0L, 1000000L, False,
	                            XA_ATOM, &retType, &retFormat, &n, &bytesLeft, 
			            (unsigned char**)&protocols);
	    if( ret == Success ){
#ifdef NXWIN_MULTIWINDOW_DEBUG
	         ErrorF("winMultiWindowWMProc: GetWindowProperty returned %lu\n", n);     	    
#endif
		 my_protocols = malloc( n * sizeof(Atom) );
		 if(!my_protocols)
	              FatalError("winMultiWindowWMProc: malloc failed\n");
		 memcpy(my_protocols, protocols, n * sizeof(Atom));		 
#ifdef NXWIN_MULTIWINDOW_DEBUG
	         ErrorF("winMultiWindowWMProc: WM_DELETE = %d\n", pWMInfo->atmWmDelete);
#endif
		 for (i = 0; i < n; ++i){
#ifdef NXWIN_MULTIWINDOW_DEBUG
		      ErrorF("winMultiWindowWMProc: protocols[%d] = %d\n", i, my_protocols[i]);
#endif
		      if (my_protocols[i] == pWMInfo->atmWmDelete)
		           ++found;
		 }
	         /* FIXME : it was xfree */	
#ifdef NXWIN_MULTIWINDOW_DEBUG
		 ErrorF("winMultiWindowWMProc: Calling free(protocols) after GetWindowProperty\n");
#endif
		 free (my_protocols);
		 /*
		   free (protocols);
		  */  
	    }
#ifdef NXWIN_MULTIWINDOW_DEBUG 
	    else{
	         ErrorF("winMultiWindowWMProc: GetWindowProperty ret %d, found is %d\n", ret, found);
		 ErrorF("   pWin = %p\n   prop = %d\n",pNode->msg.pWin, prop);
	    }
#endif

#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	         ErrorF("winMultiWindowWMProc: UNLOCK after GetWindowProperty\n");
#endif
                 if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultiwindowMutex))
		      ErrorF("winMultiWindowWMProc: !!! pthread_mutex_unlock failed\n");
#endif
	    /* --- */
	    if (found)
	    {
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
		    if(nxwinMultiwindow)
		         ErrorF("winMultiWindowWMProc: LOCK Before Sending Event WMDelete to client\n");
		    else
		         ErrorF("winMultiWindowWMProc: Before Sending Event WMDelete to client\n");
#endif
	            if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultiwindowMutex)) 
		              ErrorF("winMultiWindowWMProc: !!! pthread_mutex_lock failed\n");
#endif
		    NXSendClientEventToWindow(winGetClientPriv(pNode->msg.pWin), 
				              pNode->msg.iWindow, 
					      pWMInfo->atmWmProtos,
				              pWMInfo->atmWmDelete);
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
		    if(nxwinMultiwindow)
 	                 ErrorF("winMultiWindowWMProc: UNLOCK after NXSendClientEventToWindow\n");
		    else
			 ErrorF("winMultiWindowWMProc: after NXSendClientEventToWindow\n");
#endif
	            if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultiwindowMutex))
		              ErrorF("winMultiWindowWMProc: !!! pthread_mutex_unlock failed\n");
#endif
	    }
#if 0
#ifdef NXWIN_MULTIWINDOW
	    else
	    {
#ifdef NXWIN_MULTIWINDOW_DEBUG
	            if(nxwinMultiwindow)	    
	                 ErrorF("winMultiWindowWMProc: LOCK before Killing Client\n");
	            else
			 ErrorF("winMultiWindowWMProc: before Killing Client\n");   
#endif
	            if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultiwindowMutex))
		         ErrorF("winMultiWindowWMProc: !!! pthread_mutex_lock failed\n");
#endif
                    CloseDownClient(winGetClientPriv(pNode->msg.pWin));
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	            if(nxwinMultiwindow)	    
	                 ErrorF("winMultiWindowWMProc: UNLOCK before after Client\n");
	            else
			 ErrorF("winMultiWindowWMProc: after Killing Client\n");   
#endif
	            if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultiwindowMutex))
		              ErrorF("winMultiWindowWMProc: !!! pthread_mutex_unlock failed\n");
	            /*
	             XKillClient (pWMInfo->pDisplay,
			   pNode->msg.iWindow);
	             */
#endif
	    }
#endif
	  }
#ifdef NXWIN_MULTIWINDOW_DEBUG
	  ErrorF ("winMultiWindowWMProc: WM_WM_KILL end\n");
#endif
	  break;

	case WM_WM_ACTIVATE:
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\tWM_WM_ACTIVATE\n");
#endif
	  
	  /* Set the input focus */
	  /*
	  XSetInputFocus (pWMInfo->pDisplay,
			  pNode->msg.iWindow,
			  RevertToPointerRoot,
			  CurrentTime);
	  */		
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	  if(nxwinMultiwindow)
	       ErrorF("winMultiWindowWMProc: LOCK before SetInputFocus\n");
	  else
	       ErrorF("winMultiWindowWMProc: before SetInputFocus\n");
#endif
	  if(nxwinMultiwindow && pthread_mutex_lock(&nxwinMultiwindowMutex))
	       ErrorF("!!! pthread_mutex_lock failed\n");
#endif
          /*
           *  SetInputFocus is called by Dispatch in
           *  order to avoid potential race conditions.
           *
           *  SetInputFocus (winGetClientPriv(pNode->msg.pWin), winGetinputInfokeyboard(),
           *                     pNode->msg.iWindow, RevertToPointerRoot, CurrentTime, False);
           *
           */

          nxwinSetInputFocusFlag = 1;
          nxwinSetInputFocusFocus = pNode -> msg.iWindow;
          nxwinSetInputFocusClient = CLIENT_ID(nxwinSetInputFocusFocus);
 
#ifdef NXWIN_MULTIWINDOW
#ifdef NXWIN_MULTIWINDOW_DEBUG
	  if(nxwinMultiwindow)
	       ErrorF("winMultiWindowWMProc: UNLOCK after SetInputFocus\n");
	  else
	       ErrorF("winMultiWindowWMProc: after SetInputFocus\n");
#endif
	  if(nxwinMultiwindow && pthread_mutex_unlock(&nxwinMultiwindowMutex))
               ErrorF("!!! pthread_mutex_unlock failed\n");
#endif
	  break;
#if 0
	case WM_WM_X_EVENT:
	  /* Process all X events in the Window Manager event queue */
#if CYGMULTIWINDOW_DEBUG
	  ErrorF ("\tWM_WM_X_EVENT\n");
#endif
	  FlushXEvents (pWMInfo);
	  break;
#endif
	  
	default:
	  ErrorF ("winMultiWindowWMProc - Unknown Message.\n");
	  pthread_exit (NULL);
	  break;
	}

      /* Free the retrieved message */
      free (pNode);

      /* Flush any pending events on our display */
      /* Flush (pWMInfo->pDisplay); */
    }

  /* Free the condition variable */
  pthread_cond_destroy (&pWMInfo->wmMsgQueue.pcNotEmpty);
  
  /* Free the mutex variable */
  pthread_mutex_destroy (&pWMInfo->wmMsgQueue.pmMutex);
  
  /* Free the passed-in argument */
  free (pProcArg);
  
#if CYGMULTIWINDOW_DEBUG
  ErrorF("-winMultiWindowWMProc ()\n");
#endif
}


/*
 * winInitWM - Entry point for the X server to spawn
 * the Window Manager thread.  Called from
 * winscrinit.c/winFinishScreenInitFB ().
 */

Bool
winInitWM (void **ppWMInfo,
	   pthread_t *ptWMProc,
	   pthread_mutex_t *ppmServerStarted,
	   int dwScreen)
{
  WMProcArgPtr		pArg = (WMProcArgPtr)malloc (sizeof(WMProcArgRec));
  WMInfoPtr		pWMInfo = (WMInfoPtr)malloc (sizeof(WMInfoRec));
  
  /* Bail if the input parameters are bad */
  if (pArg == NULL || pWMInfo == NULL)
    {
      ErrorF ("winInitWM - malloc fail.\n");
      return FALSE;
    }
  
  /* Set a return pointer to the Window Manager info structure */
  *ppWMInfo = pWMInfo;

  /* Setup the argument structure for the thread function */
  pArg->dwScreen = dwScreen;
  pArg->pWMInfo = pWMInfo;
  pArg->ppmServerStarted = ppmServerStarted;
  
  /* Intialize the message queue */
  ErrorF("winInitWM starting InitQueue\n");
  if (!InitQueue (&pWMInfo->wmMsgQueue))
    {
      ErrorF ("winInitWM - InitQueue () failed.\n");
      return FALSE;
    }
  
  /* Spawn a thread for the Window Manager */
  ErrorF("winInitWM spawning winMultiWindowWMProc\n");
  if (pthread_create (ptWMProc, NULL, winMultiWindowWMProc, pArg))
    {
      /* Bail if thread creation failed */
      ErrorF ("winInitWM - pthread_create failed.\n");
      return FALSE;
    }

#if CYGDEBUG || YES
  ErrorF ("winInitWM - Returning.\n");
#endif

  return TRUE;
}


/*
 * winInitMultiWindowWM - 
 */
/* FIXME: NOCLIPBOARD */
/*
Bool
winClipboardDetectUnicodeSupport ();
*/

static void
winInitMultiWindowWM (WMInfoPtr pWMInfo, WMProcArgPtr pProcArg)
{
/*
  int                   iRetries = 0;
*/
  char			pszDisplay[512];
  int			iReturn;
  /* FIXME: NOCLIPBOARD */
  /*
  Bool			fUnicodeSupport;
  */

  /* Check that argument pointer is not invalid */
  if (pProcArg == NULL)
    {
      ErrorF ("winInitMultiWindowWM - pProcArg is NULL, bailing.\n");
      pthread_exit (NULL);
    }

  ErrorF ("winInitMultiWindowWM - Calling pthread_mutex_lock ()\n");

  /* Grab our garbage mutex to satisfy pthread_cond_wait */
  iReturn = pthread_mutex_lock (pProcArg->ppmServerStarted);
  if (iReturn != 0)
    {
      ErrorF ("winInitMultiWindowWM - pthread_mutex_lock () failed: %d\n",
	      iReturn);
      pthread_exit (NULL);
    }

  ErrorF ("winInitMultiWindowWM - pthread_mutex_lock () returned.\n");

  /* FIXME: NOCLIPBOARD */
  /* Do we have Unicode support? */
  /*
  fUnicodeSupport = winClipboardDetectUnicodeSupport ();
  */
  /* Set the current locale?  What does this do? */
  /*
  if (fUnicodeSupport && !g_fCalledSetLocale)
    {
      ErrorF ("winInitMultiWindowWM - Calling setlocale ()\n");
      if (!setlocale (LC_ALL, ""))
	{
	  ErrorF ("winInitMultiWindowWM - setlocale () error\n");
	  pthread_exit (NULL);
	}
      ErrorF ("winInitMultiWindowWM - setlocale () returned\n");
  */    
      /* See if X supports the current locale */
  /*
      if (XSupportsLocale () == False)
	{
	  ErrorF ("winInitMultiWindowWM - Locale not supported by X\n");
	  pthread_exit (NULL);
	}
    }
  */
  /* FIXME: NOCLIPBOARD */
  /* Flag that we have called setlocale */
  g_fCalledSetLocale = TRUE;
  
  /* Release the garbage mutex */
  pthread_mutex_unlock (pProcArg->ppmServerStarted);

  ErrorF ("winInitMultiWindowWM - pthread_mutex_unlock () returned.\n");
#if 0
  /* Allow multiple threads to access Xlib */
  if (XInitThreads () == 0)
    {
      ErrorF ("winInitMultiWindowWM - XInitThreads () failed.\n");
      pthread_exit (NULL);
    }
  
  ErrorF ("winInitMultiWindowWM - XInitThreads () returned.\n");
#endif 
#if 0
  /* Set jump point for Error exits */
  iReturn = setjmp (g_jmpEntry);
  
  /* Check if we should continue operations */
  if (iReturn != WIN_JMP_ERROR_IO
      && iReturn != WIN_JMP_OKAY)
    {
      /* setjmp returned an unknown value, exit */
      ErrorF ("winInitMultiWindowWM - setjmp returned: %d exiting\n",
	      iReturn);
      pthread_exit (NULL);
    }
  else if (iReturn == WIN_JMP_ERROR_IO)
    {
      ErrorF ("winInitMultiWindowWM - setjmp returned WIN_JMP_ERROR_IO\n");
    }
#endif
  /* Setup the display connection string x */
  snprintf (pszDisplay, 512, "127.0.0.1:%s.%d", display, pProcArg->dwScreen);

  /* Print the display connection string */
  ErrorF ("winInitMultiWindowWM - DISPLAY=%s\n", pszDisplay);
  
  /* Initialize Mutex */
#ifdef NXWIN_MULTIWINDOW
  pthread_mutex_init(&nxwinMultiwindowMutex, NULL);
  pthread_mutex_init(&nxwinMultStackMutex, NULL);
#endif

#if 0
  /* Open the X display */
  do
    {
      /* Try to open the display */
      pWMInfo->pDisplay = XOpenDisplay (pszDisplay);
      if (pWMInfo->pDisplay == NULL)
	{
	  ErrorF ("winInitMultiWindowWM - Could not open display, try: %d, "
		  "sleeping: %d\n\f",
		  iRetries + 1, WIN_CONNECT_DELAY);
	  ++iRetries;
	  sleep (WIN_CONNECT_DELAY);
	  continue;
	}
      else
	break;
    }
  while (pWMInfo->pDisplay == NULL && iRetries < WIN_CONNECT_RETRIES);

  /* Make sure that the display opened */
  if (pWMInfo->pDisplay == NULL)
    {
      ErrorF ("winInitMultiWindowWM - Failed opening the display, "
	      "giving up.\n\f");
      pthread_exit (NULL);
    }
#endif 

  ErrorF ("winInitMultiWindowWM - MUTEX successfully initialized.\n");
  

  /* Create some atoms */
  ErrorF("winInitMultiWindowWM: creating Atom %s\n", nxwinWM_PROTOCOLS);
  pWMInfo->atmWmProtos =  MakeAtom(nxwinWM_PROTOCOLS, strlen(nxwinWM_PROTOCOLS), True);
  /*
	  XInternAtom (pWMInfo->pDisplay, "WM_PROTOCOLS", False);
*/	  
  ErrorF("winInitMultiWindowWM: creating Atom %s\n", nxwinWM_DELETE);
  pWMInfo->atmWmDelete =  MakeAtom(nxwinWM_DELETE, strlen(nxwinWM_DELETE), True);
/*
  XInternAtom (pWMInfo->pDisplay, "WM_DELETE_WINDOW", False);
*/  
}


/*
 * winSendMessageToWM - Send a message from the X thread to the WM thread
 */

void
winSendMessageToWM (void *pWMInfo, winWMMessagePtr pMsg)
{
  WMMsgNodePtr pNode;
  
#if CYGMULTIWINDOW_DEBUG
  ErrorF ("winSendMessageToWM ()\n");
#endif
  
  pNode = (WMMsgNodePtr)malloc(sizeof(WMMsgNodeRec));
  if (pNode != NULL)
    {
      memcpy (&pNode->msg, pMsg, sizeof(winWMMessageRec));
      PushMessage (&((WMInfoPtr)pWMInfo)->wmMsgQueue, pNode);
    }
}

void
winGetWindowName(void *pWin, char **ppName)
{
  GetWindowName(pWin, ppName);
}
