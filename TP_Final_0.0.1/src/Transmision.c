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
	//Habilita interrucpción Tx

	UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, ENABLE);
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
	if (tmp == UART_IIR_INTID_THRE){
		UART_IntTransmit();
	}
	return;
}

void UART_IntTransmit(void){

	static uint8_t comando[]="ActualizarStock";
	uint8_t mensaje[15]={};
	uint8_t stock[]={};
	getStockEnAscii(stock);
	uint8_t bandera=1;

	UART_Receive(LPC_UART0, mensaje, sizeof(mensaje), BLOCKING);

	for(uint8_t i=0; i<15; i++){
		if(comando[i]!=mensaje[i]){
			bandera=0;
			break;
		}
	}

	//TODO Ver donde se modifica stock

	if(bandera) UART_Send(LPC_UART0, stock, sizeof(stock), BLOCKING);

	else UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, ENABLE);

	return;
}
