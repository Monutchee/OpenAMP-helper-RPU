#ifndef TEST_METAL_IRQ_CONTROLLER_H
#define TEST_METAL_IRQ_CONTROLLER_H

struct metal_irq {
    int (*hd)(int irq, void *arg);
    void *arg;
};

struct metal_irq_controller {
    int irq_base;
    int irq_num;
    struct metal_irq *irqs;
};

int metal_irq_register_controller(struct metal_irq_controller *controller);

static inline int metal_irq_handle(struct metal_irq *data, int irq)
{
    return data && data->hd ? data->hd(irq, data->arg) : 0;
}

#endif
