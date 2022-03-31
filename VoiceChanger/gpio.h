/*
 * gpio.h
 *
 *  Created on: 2022Äê3ÔÂ28ÈÕ
 *      Author: Tyler
 */

#ifndef GPIO_H_
#define GPIO_H_
#include "platform_test.h"

interrupt void gpioISR(void);
TEST_STATUS gpio_init(void *testArgs);



#endif /* GPIO_H_ */
