#include <Multiplex.h>
#include <logica.h>
#include "LPC17xx.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_uart.h"

//para anodo comun-revisar
uint8_t apagar[]={ 0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b10111110, 0b00000111, 0b01111111, 0b01101111};
uint8_t prender[]={0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b01000001, 0b11111000, 0b10000000, 0b10010000};
uint8_t nums[15][2]={};
uint8_t nums1[5]={9,8,7,2,5};
//uint8_t nums[15]={"012345678901234"};
uint8_t prueba[19]={"12345\t6789A\tBCDEF"};
uint8_t displayOff=0b00000000;
uint8_t primero=0;
uint8_t ultimo=4;

/* Convierte el dato recibido en un valor unitario para mostrar por GPIO en 7 segmentos
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
		prueba[dig]=parcial;
		if(dig==2||dig==6||dig==12)nums[dig][1]=SI;
		else nums[dig][1]=NO;
		dig++;
		div=div/10;
	}
	//prueba[dig+1]='\t';
	return;
}

/* Convierte el dato recibido en un valor unitario para mostrar por GPIO en 7 segmentos
 * Param:
 * 			uint32_t disp	Display seleccionado
 * 			uint8_t  dig	Digito a mostrar
 */
void display(uint32_t disp, uint8_t dig){
	uint8_t puerto=0;
	uint32_t aA=0;
	uint32_t aP=0;
	/*if(dig<12) puerto=0;
	else puerto=2;*/
	GPIO_SetValue(puerto,disp);
	GPIO_ClearValue(puerto,~disp);
	GPIO_ClearValue(0,(uint32_t)(apagar[nums1[dig]]<<4));
	GPIO_SetValue(0,(uint32_t)(prender[nums1[dig]]<<4));
	return;
}

/* Configura SYSTICK con clock interno e interrupciones
 * Param:
 * 			uint32_t time Intervalo de tiempo deseado
 */
void confSYSTICK(uint32_t time){
	SYSTICK_InternalInit(time);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
	NVIC_SetPriority(SysTick_IRQn,20);
	return;
}

/* Rutina de servicio de interrupcion de Systick
 * Param:
 * 			NONE
 */
void SysTick_Handler(void){
	static uint8_t dig = 0;
	static uint32_t disp = 0;
	static uint8_t conta=0;

	if(dig<primero||dig>ultimo) dig=primero;

	switch(conta){
		case 0: disp=(1<<0); break;
		case 1: disp=(1<<1); break;
		case 2: disp=(1<<15); break;
		case 3: disp=(1<<16); break;
		case 4: disp=(1<<17); break;
		default: break;
	}
	conta++;
	if(conta>4) conta=0;
	display(disp, dig);

	dig++;

	if(dig>14)dig=0;
	return;
}

void sendPrueba(void){
	uint8_t mensaje[]="Precio\tPeso\tMonto\n\r";
	uint8_t mensaje1[]="Codigo\tPrecio\tCantidad\n\r";
	if(!estaModificando())UART_Send(LPC_UART0, mensaje, sizeof(mensaje), BLOCKING);
	else UART_Send(LPC_UART0, mensaje1, sizeof(mensaje1), BLOCKING);
	//uint8_t mensaje1[]=""
	uint8_t aux[19]={};
	uint8_t conta=0;
	for(uint8_t i=0; i<17; i++){
		if(i==5||i==11) aux[i]='\t';
		else {
			aux[i]=nums[conta][0]+48;
			conta++;
		}
	}
	aux[17]='\n';
	aux[18]='\r';
	UART_Send(LPC_UART0, aux, sizeof(aux), BLOCKING);
	return;
}

void setPrimero(uint8_t prim){
	primero=prim;
	ultimo=prim+4;

	return;
}
