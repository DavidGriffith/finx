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

#ifndef SetUnpackColormapCompat_H
#define SetUnpackColormapCompat_H

#include "Message.h"

//
// Set the verbosity level.
//

#define PANIC
#define WARNING
#undef  TEST
#undef  DEBUG
#undef  DUMP

//
// Set default values.
//

#define SETUNPACKCOLORMAP_ENABLE_CACHE                       1
#define SETUNPACKCOLORMAP_ENABLE_DATA                        1
#define SETUNPACKCOLORMAP_ENABLE_SPLIT                       1
#define SETUNPACKCOLORMAP_ENABLE_COMPRESS                    1

#define SETUNPACKCOLORMAP_DATA_LIMIT                         4096
#define SETUNPACKCOLORMAP_DATA_OFFSET                        8

#define SETUNPACKCOLORMAP_CACHE_SLOTS                        2000
#define SETUNPACKCOLORMAP_CACHE_THRESHOLD                    5
#define SETUNPACKCOLORMAP_CACHE_LOWER_THRESHOLD              0

//
// The message class.
//

class SetUnpackColormapCompatMessage : public Message
{
  friend class SetUnpackColormapCompatStore;

  public:

  SetUnpackColormapCompatMessage()
  {
  }

  ~SetUnpackColormapCompatMessage()
  {
  }

  //
  // Put here the fields which constitute 
  // the 'identity' part of the message.
  //

  private:

  unsigned char client;
  unsigned int  entries;
};

class SetUnpackColormapCompatStore : public MessageStore
{
  public:

  SetUnpackColormapCompatStore(StaticCompressor *compressor);

  virtual ~SetUnpackColormapCompatStore();

  virtual const char *name() const
  {
    return "SetUnpackColormapCompat";
  }

  virtual unsigned char opcode() const
  {
    return X_NXSetUnpackColormap;
  }

  virtual unsigned int storage() const
  {
    return sizeof(SetUnpackColormapCompatMessage);
  }

  //
  // Message handling methods.
  //

  protected:

  virtual Message *create() const
  {
    return new SetUnpackColormapCompatMessage();
  }

  virtual Message *create(const Message &message) const
  {
    return new SetUnpackColormapCompatMessage((const SetUnpackColormapCompatMessage &) message);
  }

  virtual void destroy(Message *message) const
  {
    delete (SetUnpackColormapCompatMessage *) message;
  }

  virtual int encodeIdentity(EncodeBuffer &encodeBuffer, const unsigned char *buffer,
                                 const unsigned int size, int bigEndian,
                                     ChannelCache *channelCache) const;

  virtual int decodeIdentity(DecodeBuffer &decodeBuffer, unsigned char *&buffer,
                                 unsigned int &size, int bigEndian, WriteBuffer *writeBuffer,
                                     ChannelCache *channelCache) const;

  virtual int parseIdentity(Message *message, const unsigned char *buffer,
                                unsigned int size, int bigEndian) const;

  virtual int unparseIdentity(const Message *message, unsigned char *buffer,
                                  unsigned int size, int bigEndian) const;

  virtual void updateIdentity(EncodeBuffer &encodeBuffer, const Message *message,
                                  const Message *cachedMessage,
                                      ChannelCache *channelCache) const;

  virtual void updateIdentity(DecodeBuffer &decodeBuffer, const Message *message,
                                  ChannelCache *channelCache) const;

  virtual void identityChecksum(const Message *message, const unsigned char *buffer,
                                    unsigned int size, int bigEndian) const;

  virtual void dumpIdentity(const Message *message) const;
};

#endif /* SetUnpackColormapCompat_H */
