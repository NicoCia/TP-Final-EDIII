#include "LPC17xx.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_gpio.h"
#include "Multiplex.h"


//para catodo comun-revisar
uint8_t prender[]={0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110};
uint8_t apagar[]={0b00000011, 0b10011111, 0b00100101, 0b00001101, 0b10011001, 0b01001001, 0b01000001, 0b00011111, 0b00000001, 0b00001001};
uint8_t *nums[15]={};
uint8_t displayOff=0b00000000;

/*Convierte el dato recibido en un valor unitario para mostrar por GPIO en 7 segmentos
 * Param:
 * 			uint16_t dato Valor a convertir
 */
void convert(uint16_t dato, Bloque i){
	uint16_t resto=dato;
	uint8_t dig;

	switch(i){
		case PRECIO: dig=0; break;
		case PESO: dig=5; break;
		case MONTO: dig=10; break;
		case TECLADO: dig=10; break;
		default: break;
	}

	uint8_t parcial;
	uint16_t div=10000;

	while(div!=0){
		parcial=resto/div;
		resto=resto%div;
		nums[dig][0]=parcial;
		/*TODO
		 * implementar que al estar en bloque TECLADO, los displays que no se utilizan en el codigo se apaguen
		 */
		 /*if(i==TECLADO)
		else */if(dig==2||dig==6||dig==12)nums[dig][1]=SI;
		else nums[dig][1]=NO;
		dig++;
		div=div/10;
	}

	return;
}

/*Convierte el dato recibido en un valor unitario para mostrar por GPIO en 7 segmentos
 * Param:
 * 			uint32_t disp	Display seleccionado
 * 			uint8_t  dig	Digito a mostrar
 */
void display(uint32_t disp, uint8_t dig){
	GPIO_ClearValue(0,disp);
	GPIO_SetValue(0,~disp);
	GPIO_ClearValue(0,(uint32_t)((apagar[nums[dig][0]]+(~nums[dig][1]))<<4));
	GPIO_SetValue(0,(uint32_t)((prender[nums[dig][0]]+nums[dig][1])<<4));

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

/*Rutina de servicio de interrupcion de Systick
 * Param:
 * 			NONE
 */
void SysTick_Handler(void){
	static uint8_t dig = 0;
	static uint32_t disp=(1<<12);

	display(disp, dig);

	disp=(disp<<1);
	dig++;

	if(dig>14)dig=0;
	if(disp>(1<<26)) disp=(1<<12);

	return;
}
