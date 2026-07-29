/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * Copyright (c) 2017-2024 Xilinx, Inc. and Contributors. All rights reserved.
 * Copyright (c) 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_INFO_H_
#define PLATFORM_INFO_H_

#include <openamp/remoteproc.h>
#include <openamp/rpmsg.h>
#include <openamp/virtio.h>
#include <metal/atomic.h>
#include <metal/log.h>
#include "bspconfig.h"
#include "xparameters.h"
#include "xreg_cortexr5.h"

/*
 * Machine port: with MNC_OPENAMP_CONTRACT the shared-memory and mailbox
 * policy comes from openamp_contract.h. Hardware interrupt and peripheral
 * addresses continue to come from the XSA-generated xparameters.h.
 */
#ifdef MNC_OPENAMP_CONTRACT
#include "openamp_contract.h"
#endif

#if defined __cplusplus
extern "C" {
#endif

#ifndef IPI_IRQ_VECT_ID
#if defined(XPAR_XIPIPSU_0_INT_ID)
#define IPI_IRQ_VECT_ID XPAR_XIPIPSU_0_INT_ID
#elif defined(XPAR_XIPIPSU_0_INTR)
/*
 * SDT BSPs expose the raw GIC SPI number as XPAR_XIPIPSU_0_INTR. The
 * FreeRTOS interrupt API used by libmetal expects the local interrupt vector.
 */
#if XPAR_XIPIPSU_0_INTR >= 32
#define IPI_IRQ_VECT_ID (XPAR_XIPIPSU_0_INTR - 32U)
#else
#define IPI_IRQ_VECT_ID XPAR_XIPIPSU_0_INTR
#endif
#else
#error "No XIPIPSU interrupt ID macro found"
#endif
#endif

#ifndef POLL_BASE_ADDR
#if defined(XPAR_XIPIPSU_0_BASE_ADDRESS)
#define POLL_BASE_ADDR XPAR_XIPIPSU_0_BASE_ADDRESS
#elif defined(XPAR_XIPIPSU_0_BASEADDR)
#define POLL_BASE_ADDR XPAR_XIPIPSU_0_BASEADDR
#else
#error "No XIPIPSU base address macro found"
#endif
#endif

/*
 * Channel configuration: shared-memory carve-out and IPI channel. These
 * values are part of the remoteproc contract with the Linux/APU side and
 * MUST match the device-tree reserved-memory regions, so they may only come
 * from openamp_contract.h (or an explicit -D override). The same canonical
 * contract also renders the Linux machine-configuration domain.
 * There is deliberately NO hardcoded default: a silently stale memory map
 * corrupts the rpmsg channel at runtime. In split mode, point each core's
 * build at its own generated header.
 */
#ifndef IPI_CHN_BITMASK
#error "IPI_CHN_BITMASK missing: generate openamp_contract.h and build with MNC_OPENAMP_CONTRACT (see README.md)"
#endif

#ifndef SHARED_MEM_PA
#error "SHARED_MEM_PA missing: generate openamp_contract.h and build with MNC_OPENAMP_CONTRACT (see README.md)"
#endif

#define KICK_DEV_NAME "poll_dev"
#define KICK_BUS_NAME "generic"

#ifndef SHARED_MEM_SIZE
#error "SHARED_MEM_SIZE missing: generate openamp_contract.h and build with MNC_OPENAMP_CONTRACT (see README.md)"
#endif

#ifndef SHARED_BUF_OFFSET
#error "SHARED_BUF_OFFSET missing: generate openamp_contract.h and build with MNC_OPENAMP_CONTRACT (see README.md)"
#endif

struct remoteproc_priv {
	const char *kick_dev_name;
	const char *kick_dev_bus_name;
	struct metal_device *kick_dev;
	struct metal_io_region *kick_io;
#ifndef RPMSG_NO_IPI
	uint32_t ipi_chn_mask;
	atomic_int ipi_nokick;
#endif
};

int32_t platform_init(int32_t argc, char *argv[], void **platform);

struct rpmsg_device *
platform_create_rpmsg_vdev(void *platform, uint32_t vdev_index,
			   uint32_t role,
			   void (*rst_cb)(struct virtio_device *vdev),
			   rpmsg_ns_bind_cb ns_bind_cb);

int32_t platform_poll(void *platform);

struct rproc_plat_info {
	struct rpmsg_device *rpdev;
	struct remoteproc *rproc;
};

int32_t platform_poll_on_vdev_reset(void *arg);
void platform_release_rpmsg_vdev(struct rpmsg_device *rpdev, void *platform);
void platform_cleanup(void *platform);

#if defined __cplusplus
}
#endif

#endif /* PLATFORM_INFO_H_ */
