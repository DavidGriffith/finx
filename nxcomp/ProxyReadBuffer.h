/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXCOMP, NX protocol compression and NX extensions to this software     */
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

#ifndef ProxyReadBuffer_H
#define ProxyReadBuffer_H

#include "ReadBuffer.h"
#include "Control.h"

class ProxyReadBuffer : public ReadBuffer
{
  public:

  ProxyReadBuffer(Transport *transport)

    : ReadBuffer(transport)
  {
  }

  virtual ~ProxyReadBuffer()
  {
  }

  protected:

  virtual unsigned int suggestedLength(unsigned int pendingLength);

  virtual int locateMessage(const unsigned char *start,
                                const unsigned char *end,
                                    unsigned int &controlLength,
                                        unsigned int &dataLength,
                                            unsigned int &trailerLength);
};

#endif /* ProxyReadBuffer_H */
