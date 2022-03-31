/*
 * main.c
 */
#include "csl_dma.h"
#include "csl_intc.h"
#include <stdio.h>
#include "audio_common.h"
#include "fir.h"
#include "dma.h"
#include "irq.h"
#include "i2s.h"
#include "gpio.h"

extern int count;

extern Int16 FIRBuf[];
extern float FIR_LPF[];
extern Int16 RXdmaPingDstBuf[];
extern Int16 RXdmaPongDstBuf[];
int Codec_I2S_Init(void *testArgs);

int main(void) {
	Int16 status;
	Int16 retVal;
	void *testArgs = NULL;

	/* Initialize the platform */
	status = initPlatform();
	if(status != Platform_EOK)
	{
		printf("Systen_init Failed\n\r");
		return (-1);
	}

    C55x_msgWrite("\n********************************************\n\r");
    C55x_msgWrite(  "         AUDIO MIC IN LOOPBACK TEST    \n\r");
    C55x_msgWrite(  "********************************************\n\r");

	/* Enable clocks to all peripherals */
	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;//Peripheral Clock Gating Configuration Registers
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

    retVal =  SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                          CSL_EBSR_PPMODE_1);    // Configure Parallel Port for I2S2e External Bus Selection Registe
    retVal |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                          CSL_EBSR_SP1MODE_1);  // Serial Port mode 1 (I2S1 and GP[11:10]).

	retVal = initialise_i2c_interface(testArgs);
	Codec_I2S_Init(testArgs);
	gpio_init(testArgs);
	DMA_PingPong_Init();
	DMA_Dir_Init();

	IRQ_INit();
	while(1)
	{
	}
	//DMA_DIS();
	return (0);
}


