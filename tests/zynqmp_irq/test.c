#include <assert.h>
#include <stddef.h>
#include "irq_adapter.h"

static int init_status;
static int init_calls;
static int irq_calls;
static int last_irq;
static void *last_arg;

#ifdef MNC_LIBMETAL_XLNX_EXTENSION
static struct metal_irq irq_slots[4];
struct metal_irq_controller xlnx_irq_cntr = {32, 4, irq_slots};

int metal_irq_register_controller(struct metal_irq_controller *controller)
{
    assert(controller == &xlnx_irq_cntr);
    ++init_calls;
    return init_status;
}

static int record_irq(int irq, void *arg)
{
    ++irq_calls;
    last_irq = irq;
    last_arg = arg;
    return 1;
}
#else
int metal_xlnx_irq_init(void)
{
    ++init_calls;
    return init_status;
}

void metal_xlnx_irq_isr(void *arg)
{
    ++irq_calls;
    last_irq = (int)(uintptr_t)arg;
    last_arg = arg;
}
#endif

int main(void)
{
    assert(app_metal_irq_init() == 0);
    assert(init_calls == 1);
    init_status = -7;
    assert(app_metal_irq_init() == -7);
    assert(init_calls == 2);

#ifdef MNC_LIBMETAL_XLNX_EXTENSION
    int payload;
    irq_slots[2].hd = record_irq;
    irq_slots[2].arg = &payload;
    app_metal_irq_isr((void *)(uintptr_t)34);
    assert(irq_calls == 1 && last_irq == 34 && last_arg == &payload);

    /* Unregistered, below-range and above-range IRQs cannot dispatch. */
    app_metal_irq_isr((void *)(uintptr_t)32);
    app_metal_irq_isr((void *)(uintptr_t)31);
    app_metal_irq_isr((void *)(uintptr_t)36);
    app_metal_irq_isr((void *)(uintptr_t)UINTPTR_MAX);
    assert(irq_calls == 1);

    /* The installed Xilinx controller starts at vector zero. */
    xlnx_irq_cntr.irq_base = 0;
    app_metal_irq_isr((void *)(uintptr_t)2);
    assert(irq_calls == 2 && last_irq == 2 && last_arg == &payload);
#else
    app_metal_irq_isr((void *)(uintptr_t)34);
    assert(irq_calls == 1 && last_irq == 34);
    assert(last_arg == (void *)(uintptr_t)34);
#endif
    return 0;
}
