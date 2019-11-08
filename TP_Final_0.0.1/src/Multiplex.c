#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "Multiplex.h"


//para catodo comun-revisar
uint8_t prender[]={0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110};
uint8_t apagar[]={0b00000011, 0b10011111, 0b00100101, 0b00001101, 0b10011001, 0b01001001, 0b01000001, 0b00011111, 0b00000001, 0b00001001};

//uint16_t peso_max=5000/1024;

/*Convierte el dato recibido en un valor unitario para mostrar por GPIO en 7 segmentos
 * Param:
 * 			uint16_t dato Valor a convertir
 */
void convert(uint16_t dato, uint8_t *nums[], uint8_t i){
	uint8_t bandera=1;
	uint16_t resto=dato;//*peso_max;
	uint8_t dig=i;

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
			convert(parcial*div/10,nums,i);
			nums[punto-1][2]=NO;
			punto++;
			dig=llam;
		}
		if(div==0)bandera=0;
	}
	nums[punto][2]=SI;
	return;
}

/*Convierte el dato recibido en un valor unitario para mostrar por GPIO en 7 segmentos
 * Param:
 * 			uint8_t *nums[] Arreglo de numeros a mostrar
 * 			uint32_t disp	Display seleccionado
 * 			uint8_t  dig	Digito a mostrar
 */
void display(uint8_t *nums[], uint32_t disp, uint8_t dig){
	GPIO_ClearValue(0,disp);
	GPIO_SetValue(0,~disp);
	GPIO_ClearValue(0,(uint32_t)((apagar[nums[dig][1]]+nums[dig][2])<<4));
	GPIO_SetValue(0,(uint32_t)((prender[nums[dig][1]]+nums[dig][2])<<4));

	return;
}

/*Configura SYSTICK con clock interno e interrupciones
 * Param:
 * 			uint32_t time Intervalo de tiempo deseado
 */
void confSYSTICK(uint32_t time){
	SYSTICK_InternalInit(time);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
}
