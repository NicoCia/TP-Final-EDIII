/*
 * GPIO.h
 *
 *  Created on: 13 de nov. de 2019
 *      Author: nicoc
 */

#ifndef GPIO_H_
#define GPIO_H_
#include "LPC17xx.h"

extern void confGPIO(void);
extern void confPinSal(uint8_t puerto,uint8_t primerBit, uint8_t ultimoBit);
extern void confPinSoC(uint8_t puerto,uint8_t primerBit, uint8_t ultimoBit,uint8_t valor);

#endif /* GPIO_H_ */
