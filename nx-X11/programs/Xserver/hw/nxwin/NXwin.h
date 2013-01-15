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

/*
 * When the NX_TRANS_SOCKET is defined we use the _XSelect() function 
 * instead of the select() (see the Xpoll.h file for details). To avoid
 * this and preserve the independence of the NXWin (which doens't need the
 * Xtransport) from the X11 libs we use the following stub.
 */

#include <sys/select.h>

int _XSelect(int maxfds, fd_set *readfds, fd_set *writefds,
                 fd_set *exceptfds, struct timeval *timeout)
{
    return select(maxfds, readfds, writefds, exceptfds, timeout);
}


/*
 * The NXWin doesn't need the following variable (used by the Popen()
 * function defined in the libos), so we can declare it here to
 * preserve the independence of the NXWin from the X11 libs.
 */

int _NXUnsetLibraryPath;

