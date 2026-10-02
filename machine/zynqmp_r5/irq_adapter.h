/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef MNC_ZYNQMP_R5_IRQ_ADAPTER_H
#define MNC_ZYNQMP_R5_IRQ_ADAPTER_H

#include <stdint.h>
#include <metal/sys.h>

#ifdef MNC_LIBMETAL_XLNX_EXTENSION
#include <metal/irq_controller.h>

/* Vitis 2026.1 supplies the controller in libmetal_xlnx_extension; the
 * application registers it and forwards the FreeRTOS IPI callback. */
extern struct metal_irq_controller xlnx_irq_cntr;

static inline int app_metal_irq_init(void)
{
	return metal_irq_register_controller(&xlnx_irq_cntr);
}

static inline void app_metal_irq_isr(void *arg)
{
	uintptr_t vector = (uintptr_t)arg;
	uintptr_t base = (uintptr_t)xlnx_irq_cntr.irq_base;

	if (vector < base || vector - base >= (uintptr_t)xlnx_irq_cntr.irq_num)
		return;
	(void)metal_irq_handle(&xlnx_irq_cntr.irqs[vector - base], (int)vector);
}
#else
/* Older BSPs keep the Xilinx controller and dispatch inside libmetal. */
static inline int app_metal_irq_init(void)
{
	return metal_xlnx_irq_init();
}

static inline void app_metal_irq_isr(void *arg)
{
	metal_xlnx_irq_isr(arg);
}
#endif

#endif /* MNC_ZYNQMP_R5_IRQ_ADAPTER_H */
