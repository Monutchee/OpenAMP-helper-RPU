#ifndef TEST_METAL_SYS_H
#define TEST_METAL_SYS_H

#ifndef MNC_LIBMETAL_XLNX_EXTENSION
int metal_xlnx_irq_init(void);
void metal_xlnx_irq_isr(void *arg);
#endif

#endif
