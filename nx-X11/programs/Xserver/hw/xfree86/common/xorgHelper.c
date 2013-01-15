/* $XdotOrg: xc/programs/Xserver/hw/xfree86/common/xorgHelper.c,v 1.6 2005/07/03 08:53:42 daniels Exp $ */

#ifdef HAVE_XORG_CONFIG_H
#include <xorg-config.h>
#endif

#include <X11/X.h>
#include "os.h"
#include "servermd.h"
#include "pixmapstr.h"
#include "windowstr.h"
#include "propertyst.h"
#include "gcstruct.h"
#include "loaderProcs.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xorgVersion.h"


CARD32
xorgGetVersion()
{
    return XORG_VERSION_CURRENT;
}
