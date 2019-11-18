/*
 * Transmision.c
 *
 *  Created on: 13 de nov. de 2019
 *      Author: nicoc
 */
#include <Transmision.h>
#include <logica.h>
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
//#include "lpc17xx_gpio.h"

void confUART(void);
void confUARTPin(void);
void UART_IntTransmit(void);
void UART0_IRQHandler(void);
/*
 * Configura el Pin de Tx y Rx
 * Param:
 * 			NONE
 */
void confUARTPin(void){
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);
	return;
}

/*Configuracion UART0
 * Param:
 * 			NONE
 */
void confUART(void){
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	//Configuración por defecto:
	UART_ConfigStructInit(&UARTConfigStruct);
	//Inicializa periférico
	UART_Init(LPC_UART0, &UARTConfigStruct);
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	//Inicializa FIFO
	UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);
	//Habilita transmisión
	UART_TxCmd(LPC_UART0, ENABLE);
	//Habilita interrucpción Rx

	UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);
	/* preemption = 1, sub-priority = 1 */
	//NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));
	confUARTPin();
	//Habilita interrpción por UART0
	NVIC_EnableIRQ(UART0_IRQn);
	return;
}

/*Rutina de servicio de interrupcion de UART0
 * Param:
 * 			NONE
 */
void UART0_IRQHandler(void){
	uint32_t intsrc, tmp;
	//Determina la fuente de interrupción
	intsrc = UART_GetIntId(LPC_UART0);
	tmp = intsrc & UART_IIR_INTID_MASK;
	// Evalúa si Transmit Holding está vacío
	if (tmp == UART_IIR_INTID_RDA){
		UART_IntTransmit();
	}
	return;
}

void UART_IntTransmit(void){

	uint32_t conta=0;
	uint8_t mensaje[1]="";
	uint8_t stock[1500]="";
	getStockEnAscii(stock);
	//uint8_t error[]={"\r\nIngrese 's' para obtener el stock\n\r"};
	uint8_t bandera=1;

	UART_Receive(LPC_UART0, mensaje, sizeof(mensaje), BLOCKING);

	switch(mensaje[0]){
		case'0': bandera=0; accion(0); break;
		case'1': bandera=0; accion(1); break;
		case'2': bandera=0; accion(2); break;
		case'3': bandera=0; accion(3); break;
		case'4': bandera=0; accion(4); break;
		case'5': bandera=0; accion(5); break;
		case'6': bandera=0; accion(6); break;
		case'7': bandera=0; accion(7); break;
		case'8': bandera=0; accion(8); break;
		case'9': bandera=0; accion(9); break;
		case'A': bandera=0; accion(10); break;
		case'B': bandera=0; accion(11); break;
		case'C': bandera=0; accion(12); break;
		case'D': bandera=0; accion(13); break;
		case'*': bandera=0; accion(14); break;
		case'#': bandera=0; accion(15); break;
		case'S': bandera=1; break;
		default: break;
	}

	for(uint16_t i=0; i<15000; i++){
		if(stock[i]=='z') break;
		conta++;
	}
	if(bandera) UART_Send(LPC_UART0, stock, conta, BLOCKING);
	else sendPrueba();
	/*else {
		UART_Send(LPC_UART0, error, sizeof(error), BLOCKING);
		UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, ENABLE);
	}*/

	return;
}
