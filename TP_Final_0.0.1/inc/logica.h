/*
 * logica.h
 *
 *  Created on: 13 de nov. de 2019
 *      Author: nicoc
 */

#ifndef LOGICA_H_
#define LOGICA_H_
#include "LPC17xx.h"

extern void getStockEnAscii(uint8_t datosEnChar[]);
extern void accion(uint8_t tecla);
extern void pruebaDemo();
extern void initTabla();
extern uint8_t estaPesando();
extern uint16_t getPrecio();

#endif /* LOGICA_H_ */
