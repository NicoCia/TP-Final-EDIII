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

/*
 * Configura el Pin de la interrupcion externa
 * Param:
 * 			uint8_t num Debe ser un número entero entre 0 y 3
 *
void confEINTPin(uint8_t num){
	if(num<4){
		PINSEL_CFG_Type PinCfg;
		PinCfg.Portnum=2;								//Puerto 2
		PinCfg.Pinnum=10+num;							//Pin 10 si EINT0, 11 si EINT1, ...
		PinCfg.Funcnum=1;								//Funcion de EINT
		PinCfg.OpenDrain=PINSEL_PINMODE_NORMAL;			//No open-drain
		PinCfg.Pinmode=PINSEL_PINMODE_TRISTATE;			//Sin resistencias de pull-up ni pull-down

		PINSEL_ConfigPin(&PinCfg);						//Configura el pin
	}
	return;
}
*/



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
	return;
}