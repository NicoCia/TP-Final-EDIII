#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "Multiplex.h"


//para catodo comun-revisar
uint8_t perder[]={0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110};
uint8_t apagar[]={0b00000011, 0b10011111, 0b00100101, 0b00001101, 0b10011001, 0b01001001, 0b01000001, 0b00011111, 0b00000001, 0b00001001};
uint8_t *nums[5]={};
//uint16_t peso_max=5000/1024;

/*Convierte el dato recibido en un valor unitario para mostrar por GPIO en 7 segmentos
 * Param:
 * 			uint16_t dato Valor a convertir
 */
uint8_t **convert(uint16_t dato){
	uint8_t bandera=1;
	uint16_t resto=dato;//*peso_max;
	uint8_t dig=0;

	uint8_t parcial;
	uint16_t div=1000;
	static uint8_t punto;
	static uint8_t llam;

	punto=0;
	llam=1;
	while(bandera){
		parcial=resto/div;
		resto=resto%div;
		if(parcial<10){
			nums[dig][1]=parcial;
			nums[dig][2]=NO;
			div=div/10;
			dig++;
		}
		else {
			llam++;
			convert(parcial*div/10);
			nums[punto-1][2]=NO;
			punto++;
			dig=llam;
		}
		if(div==0)bandera=0;
	}
	nums[punto][2]=SI;
	return nums;
}
