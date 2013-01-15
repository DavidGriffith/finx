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

typedef struct _MultStackNodeRec {
  struct _MultStackNodeRec  *pNext;
  void*                     pWin;
  unsigned long             val;
} MultStackNodeRec, *MultStackNodePtr;

typedef struct _MultStackQueueRec {
  struct _MultStackNodeRec *pHead;
  struct _MultStackNodeRec *pTail;
} MultStackQueueRec, *MultStackQueuePtr;

