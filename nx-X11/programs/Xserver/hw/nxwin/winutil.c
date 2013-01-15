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
#include "inputstr.h"

ClientPtr winGetClientPriv(WindowPtr pWin)
{
#if CYGMULTIWINDOW_DEBUG
	ErrorF("Client is %p--pWin is %p\n",winGetWindowPriv(pWin)->client,winGetWindowPriv(pWin));
#endif
	return winGetWindowPriv(pWin)->client;
}
unsigned winGetOverrideRedirectPriv(WindowPtr pWin)
{
	return pWin->overrideRedirect;
}

DeviceIntPtr winGetinputInfokeyboard()
{
	return inputInfo.keyboard;
}

