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

#ifndef Misc_H
#define Misc_H

#include <iostream.h>

#include <errno.h>
#include <string.h>

//
// This is MD5 length.
//

#define MD5_LENGTH          16

//
// Error handling macros.
//

#define ESET(e)  (errno = (e))
#define EGET()   (errno)
#define ESTR()   strerror(errno)

//
// TCP port offset applied to NX port specification.
//

extern const int DEFAULT_NX_PROXY_PORT_OFFSET;

//
// Default TCP port used by client proxy to listen
// to X clients and by server proxy to connect to
// remote.
//

extern const int DEFAULT_NX_PROXY_PORT;

//
// Default X display number that client
// proxy imitates.
//

extern const int DEFAULT_NX_X_PORT;

//
// Establish the port offsets for the additional
// services.
//

extern const int DEFAULT_NX_CUPS_PORT_OFFSET;
extern const int DEFAULT_NX_SAMBA_PORT_OFFSET;
extern const int DEFAULT_NX_MEDIA_PORT_OFFSET;
extern const int DEFAULT_NX_KEYBD_PORT_OFFSET;
extern const int DEFAULT_NX_HTTP_PORT_OFFSET;

//
// Listen on these ports by default. It should
// be the display port offset where agent is
// running plus the specific service's offset.
//

extern const int DEFAULT_NX_CUPS_PORT;
extern const int DEFAULT_NX_SAMBA_PORT;
extern const int DEFAULT_NX_MEDIA_PORT;
extern const int DEFAULT_NX_KEYBD_PORT;
extern const int DEFAULT_NX_HTTP_PORT;

//
// Connect services to these ports by default.
//

extern const int DEFAULT_CUPS_SERVER_PORT;
extern const int DEFAULT_SAMBA_SERVER_PORT;
extern const int DEFAULT_MEDIA_SERVER_PORT;
extern const int DEFAULT_KEYBD_SERVER_PORT;
extern const int DEFAULT_HTTP_SERVER_PORT;

//
// Return strings containing various info.
//

extern const char *GetUsageInfo();
extern const char *GetCopyrightInfo();
extern const char *GetOtherCopyrightInfo();

//
// Define this if you want immediate flush of
// the log output.
//

#define FLUSH

//
// Global objects providing shared functions.
//

class Auth;
class Control;
class Statistics;

extern Auth       *auth;
extern Control    *control;
extern Statistics *statistics;

//
// Log file.
//

extern ostream *logofs;

//
// Cleanup code.
//

extern "C"
{
  extern void HandleCleanup() __attribute__((noreturn));;
}

extern void HandleAbort() __attribute__((noreturn));;

//
// Manage signal handlers.
//

extern void DisableSignals();
extern void EnableSignals();

//
// Show a dialog asking user if he or she wants
// to close the current session. Look in file
// Alerts.h for predefined critical events.
//

extern void HandleAlert(int alert);

//
// Print out content of buffer to log file.
// You need to define DUMP or OPCODES in
// the source to have these compiled.
//

extern const char *DumpOpcode(const int &opcode);
extern const char *DumpChecksum(const void *checksum);

extern void DumpData(const unsigned char *data, unsigned int length);
extern void DumpHexData(const unsigned char *data, unsigned int length);
extern void DumpChecksum(const unsigned char *data, unsigned int length);
extern void DumpBlockChecksums(const unsigned char *data, unsigned int length,
                                   unsigned int block);

//
// Defines logofs_flush as an empty string to
// avoid calling the corresponding ostream's
// flush() function.
//

#ifdef FLUSH

#define logofs_flush "" ; logofs -> flush()

#else

#define logofs_flush ""

#endif

//
// Is the host where local proxy is running
// big-endian?
//

extern int _hostBigEndian;
extern int _storeBigEndian;

inline void setHostBigEndian(int flag)
{
  _hostBigEndian = flag;
}

inline int hostBigEndian()
{
  return _hostBigEndian;
}

inline int storeBigEndian()
{
  return _storeBigEndian;
}

unsigned int GetUINT(unsigned const char *buffer, int bigEndian);
unsigned int GetULONG(unsigned const char *buffer, int bigEndian);
void PutUINT(unsigned int value, unsigned char *buffer, int bigEndian);
void PutULONG(unsigned int value, unsigned char *buffer, int bigEndian);

int PutData(ostream *fs, unsigned char *buffer, int size);
int GetData(istream *fs, unsigned char *buffer, int size);

unsigned int RoundUp2(unsigned int x);
unsigned int RoundUp4(unsigned int x);
unsigned int RoundUp8(unsigned int x);

#endif /* Misc_H */
