/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXCOMPEXT, NX protocol compression and NX extensions to this software  */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/

#ifndef Jpeg_H
#define Jpeg_H

#ifdef __cplusplus
extern "C" {
#endif

extern char *JpegCompressData(
#if NeedFunctionPrototypes
    XImage*                   /* image */,
    int                       /* level */,
    int*                      /* compressed_size */
#endif
);

#ifdef __cplusplus
}
#endif

#endif /* Jpeg_H */

