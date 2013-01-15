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

#ifndef Timestamp_H
#define Timestamp_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

//
// Type used for timeout manipulation.
//

typedef struct timeval T_timestamp;

//
// Get timestamp from the system.
//

inline T_timestamp getTimestamp()
{
  struct timeval ts;

  gettimeofday(&ts, NULL);

  return ts;
}

//
// Set timestamp at the given amount of milliseconds.
//

inline T_timestamp getTimestamp(long ms)
{
  struct timeval ts;

  ts.tv_sec  = ms / 1000;
  ts.tv_usec = (ms % 1000) * 1000;

  return ts;
}

inline T_timestamp &setTimestamp(T_timestamp &ts, long ms)
{
  ts.tv_sec  = ms / 1000;
  ts.tv_usec = (ms % 1000) * 1000;

  return ts;
}

inline T_timestamp &setMinTimestamp(T_timestamp &ts, long ms)
{
  if ((ts.tv_sec * 1000 + ts.tv_usec / 1000) > ms)
  {
    ts.tv_sec  = ms / 1000;
    ts.tv_usec = (ms % 1000) * 1000;
  }

  return ts;
}

inline T_timestamp &setMinTimestamp(T_timestamp &ts1, T_timestamp &ts2)
{
  if ((ts1.tv_sec * 1000000 + ts1.tv_usec) >
           (ts2.tv_sec * 1000000 + ts2.tv_usec))
  {
    ts1.tv_sec  = ts2.tv_sec;
    ts1.tv_usec = ts2.tv_usec;
  }

  return ts1;
}

inline long getMsTimestamp(const T_timestamp &ts)
{
  return ts.tv_sec * 1000 + ts.tv_usec / 1000;
}

//
// A 0 value on both fields means that
// timestamp is invalid or not set.
//

inline T_timestamp nullTimestamp()
{
  struct timeval ts;

  ts.tv_sec  = 0;
  ts.tv_usec = 0;

  return ts;
}

inline bool isTimestamp(const T_timestamp &ts)
{
  if (ts.tv_sec == 0 && ts.tv_usec == 0)
  {
     return 0;
  }

  return 1;
}

//
// Returns difference in milliseconds
// between the two timestamps.
//

inline long diffTimestamp(const T_timestamp &ts1, const T_timestamp &ts2)
{
  if (ts1.tv_sec == 0 && ts1.tv_usec == 0)
  {
     return -1;
  }

  //
  // Adds 500 microseconds to round up
  // to the nearest millisecond.
  //

  return ((ts2.tv_sec - ts1.tv_sec) * 1000000 + 
              (ts2.tv_usec - ts1.tv_usec) + 500) / 1000;
}

//
// The same in microseconds. It doesn't
// round the value.
//

inline long diffUsTimestamp(const T_timestamp &ts1, const T_timestamp &ts2)
{
  if (ts1.tv_sec == 0 && ts1.tv_usec == 0)
  {
     return -1;
  }

  return (ts2.tv_sec - ts1.tv_sec) * 1000000 + 
              (ts2.tv_usec - ts1.tv_usec);
}

inline void subMsToTimestamp(T_timestamp &ts, long ms)
{
  long sec = ms / 1000;

  ts.tv_sec -= sec;

  long mod = ms % (ms * 1000);

  if (ts.tv_usec >= (mod * 1000))
  {
    ts.tv_sec -= mod * 1000;
  }
  else
  {
    ts.tv_sec--;

    ts.tv_usec = 1000000 - mod * 1000;
  }
}

inline void addMsToTimestamp(T_timestamp &ts, long ms)
{
  long sec = ms / 1000;

  ts.tv_sec += sec;

  long mod = ms % (ms * 1000);

  ts.tv_usec += mod * 1000;

  if (ts.tv_usec > 1000000)
  {
    ts.tv_sec += 1;
  }
}

//
// Check the difference between timestamps. Return
// 0 if system time is going backward or difference
// exceeds the given number of milliseconds.
//

inline int checkDiffTimestamp(const T_timestamp &ts1, const T_timestamp &ts2,
                                  long ms = 30000)
{
  long diffTs = diffTimestamp(ts1, ts2);

  if (diffTs < 0 || diffTs > ms)
  {
    return 0;
  }

  return 1;
}

//
// All use the ctime static buffer
// from C library.
// 
//

inline char *strTimestamp(const T_timestamp &ts)
{
  char *ctime_now = ctime((time_t *) &ts.tv_sec);

  ctime_now[24] = '\0';

  return ctime_now;
}

inline char *strTimestamp()
{
  T_timestamp ts = getTimestamp();

  return strTimestamp(ts);
}

//
// This is especially dirty. 
//

inline char *strMsTimestamp(const T_timestamp &ts)
{
  char *ctime_now = ctime((time_t *) &ts.tv_sec);

  char ctime_new[25];

  sprintf(ctime_new, "%.8s:%3.3f", ctime_now + 11,
              (float) ts.tv_usec / 1000);

  strncpy(ctime_now, ctime_new, 24);

  return ctime_now;
}

inline char *strMsTimestamp()
{
  T_timestamp ts = getTimestamp();

  return strMsTimestamp(ts);
}

#endif /* Timestamp_H */
