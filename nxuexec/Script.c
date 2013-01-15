/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com.          */
/*                                                                        */
/* NXUEXEC, NX protocol compression and NX extensions to this software    */
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

#include "Main.h"

int nxuexecScriptIsTextScript(const char *filename)
{
  FILE *script;

  if (filename == 0)
  {
    return 0;
  }

  if ((script = fopen (filename, "r")) == 0)
  {
    fprintf(stderr,"Warning: Cannot open '%s': Error is %d '%s'.\n",
                filename, errno, strerror(errno));

    return 0;
  }

  if (fgetc(script) != '#')
  {
    fclose(script);

    return 0;
  }

  if (fgetc(script) != '!')
  {
    fclose(script);

    return 0;
  }

  if (fgetc(script) != '/')
  {
    fclose(script);

    return 0;
  }

  fclose(script);

  return 1;
}
