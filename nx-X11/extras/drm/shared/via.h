/*
 * Copyright 1998-2003 VIA Technologies, Inc. All Rights Reserved.
 * Copyright 2001-2003 S3 Graphics, Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * VIA, S3 GRAPHICS, AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef __VIA_H__
#define __VIA_H__

#define DRM(x) viadrv_##x

#define DRIVER_AUTHOR	"VIA"

#define DRIVER_NAME		"via"
#define DRIVER_DESC		"VIA Unichrome / Pro"
#define DRIVER_DATE		"20050814"

#define DRIVER_MAJOR		2
#define DRIVER_MINOR		6
#define DRIVER_PATCHLEVEL	7

#define DRIVER_IOCTLS							\
        [DRM_IOCTL_NR(DRM_IOCTL_VIA_ALLOCMEM)]  = { via_mem_alloc,  1, 0 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_FREEMEM)]   = { via_mem_free,   1, 0 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_AGP_INIT)]   = { via_agp_init,  1, 1 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_FB_INIT)]   = { via_fb_init,    1, 1 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_MAP_INIT)]   = { via_map_init,  1, 1 }, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_DEC_FUTEX)] = { via_decoder_futex, 1, 0}, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_DMA_INIT)] = { via_dma_init, 1, 0}, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_CMDBUFFER)] = { via_cmdbuffer, 1, 0}, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_FLUSH)] = { via_flush_ioctl, 1, 0}, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_PCICMD)] = {via_pci_cmdbuffer, 1, 0}, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_CMDBUF_SIZE)] = {via_cmdbuf_size, 1, 0}, \
	[DRM_IOCTL_NR(DRM_IOCTL_VIA_WAIT_IRQ)] = {via_wait_irq, 1, 0}


#endif
