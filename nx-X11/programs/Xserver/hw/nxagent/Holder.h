/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXAGENT, NX protocol compression and NX extensions to this software    */
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

#ifndef __Holder_H__
#define __Holder_H__

void nxagentMarkPlaceholderNotLoaded(int depth);
void nxagentInitPlaceholder(int depth);
void nxagentApplyPlaceholder(Drawable drawable, int x, int y, int w, int h, int depth);

const char *nxagentChecksum(char *data, int size);

#endif /* __Holder_H__ */
