/*
 * irq.c
 *
 *  Created on: 2022��3��25��
 *      Author: Tyler
 */
#include "csl_intc.h"
#include "dma.h"
#include "gpio.h"
#include "irq.h"
extern void VECSTART(void);

void IRQ_INit(void)
{
    IRQ_globalDisable();

	IRQ_clearAll();

	IRQ_disableAll();

	IRQ_setVecs((Uint32)&VECSTART);
	IRQ_clear(DMA_EVENT);

	IRQ_plug (DMA_EVENT, &dma_isr);
    IRQ_clear(GPIO_EVENT);

    IRQ_plug(GPIO_EVENT,&gpioISR);
	IRQ_enable(DMA_EVENT);
    IRQ_enable(GPIO_EVENT);
	IRQ_globalEnable();
}

void IRQ_DIs(void)
{
	IRQ_clearAll();
	IRQ_disableAll();
	IRQ_globalDisable();
}
