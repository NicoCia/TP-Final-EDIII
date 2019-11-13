/*
 * Pesaje.h
 *
 *  Created on: 13 de nov. de 2019
 *      Author: nicoc
 */

#ifndef PESAJE_H_
#define PESAJE_H_
#include "LPC17xx.h"

extern void confADC(void);
extern uint16_t getPeso(void);
extern void tarar(void);

#endif /* PESAJE_H_ */
