/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXCOMPSH, NX protocol compression, NX extensions to this software      */
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

#ifndef Socket_H
#define Socket_H

//
// Set socket options.
//

int SetReuseAddress(int fd);
int SetNonBlocking(int fd, int value);
int SetLingerTimeout(int fd, int timeout);
int SetSendBuffer(int fd, int size);
int SetReceiveBuffer(int fd, int size);
int SetNoDelay(int fd, int value);
int SetKeepAlive(int fd);
int SetLowDelay(int fd);
int SetCloseOnExec(int fd);

//
// Get kernel support level.
//

int GetKernelStep();

//
// Get socket info.
//

int GetBytesReadable(int fd);
int GetBytesWritable(int fd);
int GetBytesQueued(int fd);

//
// Query Internet address.
//

int GetHostAddress(const char *name);

#endif /* Socket_H */
