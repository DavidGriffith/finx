/*
 * Copyright 2005 Adam Jackson.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * ADAM JACKSON BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* derived from tdfx_drv.c */

#include <linux/config.h>
#include "drmP.h"
#include "imagine_drv.h"

#include "drm_pciids.h"

static struct drm_driver driver;

static struct pci_device_id pciidlist[] = {
    imagine_PCI_IDS
};

static int probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    return drm_get_dev(pdev, ent, &driver);
}

static struct drm_driver driver = {
    .driver_features = DRIVER_USE_MTRR,
    .reclaim_buffers = drm_core_reclaim_buffers,
    .get_map_ofs = drm_core_get_map_ofs,
    .get_reg_ofs = drm_core_get_reg_ofs,
    .fops = {
        .owner = THIS_MODULE,
        .open = drm_open,
        .release = drm_release,
        .ioctl = drm_ioctl,
        .mmap = drm_mmap,
        .poll = drm_poll,
        .fasync = drm_fasync,
    },
    .pci_driver = {
        .name = DRIVER_NAME,
        .id_table = pciidlist,
        .probe = probe,
        .remove = __devexit_p(drm_cleanup_pci),
    },

    .name = DRIVER_NAME,
    .desc = DRIVER_DESC,
    .date = DRIVER_DATE,
    .major = DRIVER_MAJOR,
    .minor = DRIVER_MINOR,
    .patchlevel = DRIVER_PATCHLEVEL,
};

static int __init imagine_init(void)
{
    return drm_init(&driver, pciidlist);
}

static void __exit imagine_exit(void)
{
    drm_exit(&driver);
}

module_init(imagine_init);
module_exit(imagine_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL and additional rights");
