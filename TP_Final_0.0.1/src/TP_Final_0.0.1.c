#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "Multiplex.h"
#include "Pesaje.h"

void confEINT(void);
void confUART(void);
void confGPIO(void);

void confUARTPin(void);
void confEINTPin(uint8_t num);

void UART0_IRQHandler(void);

void UART_IntTransmit(void);

#define SALIDA (uint8_t) 1




int main(void) {


//	while(1) {
//        i++ ;
//    }
    return 0 ;
}


/*
 * Configura el Pin de la interrupcion externa
 * Param:
 * 			uint8_t num Debe ser un número entero entre 0 y 3
 */
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

/*Configuracion GPIO
 * Param:
 * 			NONE
 */
void confGPIO(void){
	confPinSal(0,4,11);
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
	//uint8_t bandera=1;

	UART_Receive(LPC_UART0, mensaje, sizeof(mensaje), BLOCKING);

	for(uint8_t i=0; i<15; i++){
		if(comando[i]!=mensaje[i]){
			//bandera=0;
			break;
		}
	}

	/*TODO Ver donde se modifica stock

	if(bandera) UART_Send(LPC_UART0, stock, sizeof(stock), NONE_BLOCKING);

	else UART_IntConfig(LPC_UART0, UART_INTCFG_THRE, ENABLE);*/

	return;
}
