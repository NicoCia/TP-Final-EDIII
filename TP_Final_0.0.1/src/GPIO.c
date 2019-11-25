/*
 * GPIO.c
 *
 *  Created on: 13 de nov. de 2019
 *      Author: nicoc
 */
#include <GPIO.h>
#include "LPC17xx.h"
#include "lpc17xx_gpio.h"

//void confEINT(void);
void confGPIO(void);
void confEINTPin(uint8_t num);
void confPinSal(uint8_t puerto,uint8_t primerBit, uint8_t ultimoBit);
void confPinSoC(uint8_t puerto,uint8_t primerBit, uint8_t ultimoBit,uint8_t valor);

#define SALIDA (uint8_t) 1

/*Configura como salidas pines contiguos pasados por parámteros
 * Param:
 * 			uint8_t puerto 	   Puerto a configurar. Debe ser un valor entre 0 y 3
 * 			uint8_t primerBit  Primer pin a configurar. Debe ser un valor entre 0 y 32
 * 			uint8_t ultimoBit  Ultimo bit a configurar. Debe ser un valor entre 0 y 32, y mayor a primerBit
 *
 */
void confPinSal(uint8_t puerto,uint8_t primerBit, uint8_t ultimoBit){
	uint32_t aux;
	if(puerto<4&&primerBit<ultimoBit&&primerBit<32&&ultimoBit<32){
		for(uint8_t i=primerBit;i<ultimoBit+1;i++){
			aux+=(1<<i);
		}
		GPIO_SetDir(puerto,aux,SALIDA);
	}
	return;

}

/*Setea o limpia pines contiguos pasados por parámteros
 * Param:
 * 			uint8_t puerto 	   Puerto a configurar. Debe ser un valor entre 0 y 3
 * 			uint8_t primerBit  Primer pin a configurar. Debe ser un valor entre 0 y 32
 * 			uint8_t ultimoBit  Ultimo bit a configurar. Debe ser un valor entre 0 y 32, y mayor a primerBit
 * 			uint8_t valor	   Limpia si valor=0, Setea si valor=1;
 */
void confPinSoC(uint8_t puerto,uint8_t primerBit, uint8_t ultimoBit,uint8_t valor){
	uint32_t aux;
	if(puerto<4&&primerBit<ultimoBit&&primerBit<32&&ultimoBit<32){		//Realiza controles

		for(uint8_t i=primerBit;i<ultimoBit+1;i++){
			aux+=(1<<i);
		}

		if		(valor==0)	GPIO_ClearValue(puerto,aux);
		else if	(valor==1)	GPIO_SetValue(puerto,aux);
	}
	return;

}



/*Configuracion GPIO
 * Param:
 * 			NONE
 */
void confGPIO(void){
	confPinSal(0,4,11);
	confPinSal(0,0,1);
	confPinSal(0,15,18);
	confPinSal(0,21,22);
	confPinSal(0,27,28);
	confPinSal(0,30,31);
	confPinSal(2,10,12);
	return;
}
