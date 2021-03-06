/* i830_drv.c -- I810 driver -*- linux-c -*-
 * Created: Mon Dec 13 01:56:22 1999 by jhartmann@precisioninsight.com
 *
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Rickard E. (Rik) Faith <faith@valinux.com>
 *    Jeff Hartmann <jhartmann@valinux.com>
 *    Gareth Hughes <gareth@valinux.com>
 *    Abraham vd Merwe <abraham@2d3d.co.za>
 *    Keith Whitwell <keith@tungstengraphics.com>
 */

#include <linux/config.h>

#include "drmP.h"
#include "drm.h"
#include "i830_drm.h"
#include "i830_drv.h"

#include "drm_pciids.h"


static struct pci_device_id pciidlist[] = {
	i830_PCI_IDS
};

extern drm_ioctl_desc_t i830_ioctls[];
extern int i830_max_ioctl;

static int probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static struct drm_driver driver = {
	.driver_features =
	    DRIVER_USE_AGP | DRIVER_REQUIRE_AGP | DRIVER_USE_MTRR |
	    DRIVER_HAVE_DMA | DRIVER_DMA_QUEUE,
#if USE_IRQS
	.driver_features |= DRIVER_HAVE_IRQ | DRIVER_SHARED_IRQ,
#endif
	.dev_priv_size = sizeof(drm_i830_buf_priv_t),
	.load = i830_driver_load,
	.lastclose = i830_driver_lastclose,
	.preclose = i830_driver_preclose,
	.device_is_agp = i830_driver_device_is_agp,
	.reclaim_buffers_locked = i830_driver_reclaim_buffers_locked,
	.dma_quiescent = i830_driver_dma_quiescent,
	.get_map_ofs = drm_core_get_map_ofs,
	.get_reg_ofs = drm_core_get_reg_ofs,
#if USE_IRQS
	.irq_preinstall = i830_driver_irq_preinstall,
	.irq_postinstall = i830_driver_irq_postinstall,
	.irq_uninstall = i830_driver_irq_uninstall,
	.irq_handler = i830_driver_irq_handler,
#endif
	.ioctls = i830_ioctls,
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

static int probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	return drm_get_dev(pdev, ent, &driver);
}


static int __init i830_init(void)
{
	driver.num_ioctls = i830_max_ioctl;
	return drm_init(&driver, pciidlist);
}

static void __exit i830_exit(void)
{
	drm_exit(&driver);
}

module_init(i830_init);
module_exit(i830_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL and additional rights");
