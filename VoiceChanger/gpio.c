/*
 * gpio.c
 *
 *  Created on: 2022年3月28日
 *      Author: Tyler
 */

#include "platform_internals.h"
#include "platform_test.h"
#include "gpio.h"
extern CSL_DMA_Handle 		dmaHandle;
extern CSL_DMA_Handle 		dir_dmaHandle;
/* Global Structure Declaration */
CSL_GpioObj     GpioObj;
CSL_GpioObj     *gpioHandle;
CSL_Status 		retVal;
volatile Uint16  flag[3]={0,0,0};

TEST_STATUS gpio_init(void *testArgs)
{
	CSL_GpioPinConfig    config;
	volatile Uint32 	 loop;

	/* Open GPIO module */
    gpioHandle = GPIO_open(&GpioObj,&retVal);
    if((NULL == gpioHandle) || (CSL_SOK != retVal))
    {
        C55x_msgWrite("GPIO_open failed\n");
        return (TEST_FAIL);
    }

	/* Reset the GPIO module */
    GPIO_reset(gpioHandle);
    for (loop=0; loop < 0x5F5E10; loop++){}
	/* GPIO_config API to make PIN12 as input pin */
	config.pinNum    = CSL_GPIO_PIN12;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioHandle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n");
	     return (TEST_FAIL);
	}
	   /** test GPIO_config API to make PIN13 as i/p */
		config.pinNum    = CSL_GPIO_PIN13;
	    config.direction = CSL_GPIO_DIR_INPUT;
	    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
	    retVal = GPIO_configBit(gpioHandle,&config);
		if(CSL_SOK != retVal)
		{
			C55x_msgWrite("test failed - GPIO_configBit\n");
		     return (TEST_FAIL);
		}

	    /** test GPIO_config API to make PIN14 as i/p */
		config.pinNum    = CSL_GPIO_PIN14;
	    config.direction = CSL_GPIO_DIR_INPUT;
	    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
	    retVal = GPIO_configBit(gpioHandle,&config);
		if(CSL_SOK != retVal)
		{
			C55x_msgWrite("test failed - GPIO_configBit\n");
		     return (TEST_FAIL);
		}
		/* Enable GPIO interrupts */
	    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN12);
		if(CSL_SOK != retVal)
		{
			C55x_msgWrite("test failed- GPIO_enableInt\n");
		     return (TEST_FAIL);
		}

		/* Enable GPIO interrupts */
	    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN13);
		if(CSL_SOK != retVal)
		{
			C55x_msgWrite("test failed- GPIO_enableInt\n");
			return(retVal);
		}

		/* Enable GPIO interrupts */
	    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN14);
		if(CSL_SOK != retVal)
		{
			C55x_msgWrite("test failed- GPIO_enableInt\n");
			return(retVal);
		}
		return (TEST_PASS);
}
interrupt void gpioISR(void)
{
   	Int16 retVal;

	//IRQ_globalDisable();
	/* Disable all the interrupts */
	//IRQ_disableAll();

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN12,&retVal)))
    {
		C55x_msgWrite("SW2 is pressed\n\r");
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN12);
        flag[0] = !flag[0];
        if (flag[0]==0)//暂停
        {
        	DMA_start (dmaHandle);
        	DMA_start (dir_dmaHandle);
        }
        else if (flag[0]==1)//继续
        {
        	DMA_stop (dmaHandle);
        	DMA_stop (dir_dmaHandle);
        }
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN13,&retVal)))
    {
		C55x_msgWrite("SW3 is pressed\n\r");
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN13);
        flag[1] = !flag[1];
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN14,&retVal)))
    {
		C55x_msgWrite("SW4 is pressed\n\r");
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN14);
        flag[2] = !flag[2];
    }

	IRQ_clear(GPIO_EVENT);
}
