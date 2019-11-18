/*
 * Pesaje.c
 *
 *  Created on: 13 de nov. de 2019
 *      Author: nicoc
 */
#include <Multiplex.h>
#include <Pesaje.h>
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"

void confTIM(void);
void confADCPin_0a3(uint8_t num);
void confADC(void);
uint16_t getPeso(void);
void tarar(void);
void ADC_IRQHanler(void);
void TIMER0_IRQHandler(void);

uint16_t mascara = 0b0000111111111100;
uint16_t resolucion = 5;
uint16_t peso=5;
static uint16_t tara = 0;

/*Configuracion Timer0 para producir un Match cada 50 ms
 * Param:
 * 			NONE
 */
void confTIM(void){
	TIM_TIMERCFG_Type TIMcfg;
	TIMcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	TIMcfg.PrescaleValue = 1;
	TIM_Init(LPC_TIM0,TIM_TIMER_MODE ,&TIMcfg);

	TIM_MATCHCFG_Type MATCHcfg;
	MATCHcfg.MatchChannel = 0;
	MATCHcfg.IntOnMatch = ENABLE;
	MATCHcfg.StopOnMatch = DISABLE;
	MATCHcfg.ResetOnMatch = ENABLE;
	MATCHcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	MATCHcfg.MatchValue = 50000;
	TIM_ConfigMatch(LPC_TIM0, &MATCHcfg);

	TIM_Cmd(LPC_TIM0, ENABLE);
	NVIC_EnableIRQ(TIMER0_IRQn);
	return;
}

/*
 * Configura el Pin del ADC, canales de 0 a 3
 * Param:
 * 			uint8_t num Debe ser un número entero entre 0 y 3
 */
void confADCPin_0a3(uint8_t num){
	if(num<4){
		PINSEL_CFG_Type PinCfg;
		PinCfg.Portnum=0;								//Puerto 0
		PinCfg.Pinnum=23+num;							//Pin 23 si AD0, 24 si AD1,...
		PinCfg.Funcnum=1;								//Funcion de ADC
		PinCfg.OpenDrain=PINSEL_PINMODE_NORMAL;			//No open drain
		PinCfg.Pinmode=PINSEL_PINMODE_PULLUP;			//Resistencia de Pull-up

		PINSEL_ConfigPin(&PinCfg);						//Configura el pin
	}
	return;
}

/*Configuracion ADC para tomar muestras cada vez que se producza un Match en Timer0
 * Param:
 * 			NONE
 */
void confADC(void){
	confADCPin_0a3(0);
	confADCPin_0a3(1);
	confADCPin_0a3(2);
	confADCPin_0a3(3);
	confTIM();
	ADC_Init(LPC_ADC, 200000);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN1, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN2, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN3, ENABLE);

	NVIC_EnableIRQ(ADC_IRQn);
	return;
}

uint16_t getPeso(void){
	return peso;
}

void tarar(void){
	tara=peso;
	return;
}

/*Rutina de servicio de interrupcion de ADC
 * Param:
 * 			NONE
 */
void ADC_IRQHanler(void){
	static uint16_t valADC1=0;
	static uint16_t valADC2=0;
	static uint16_t valADC3=0;
	static uint16_t valADC4=0;
	uint16_t dato;
	//uint16_t monto;

	if(ADC_ChannelGetStatus(LPC_ADC, 0, 1)) valADC1 = (ADC_ChannelGetData(LPC_ADC, 0)&mascara)>>2;
	else if(ADC_ChannelGetStatus(LPC_ADC, 1, 1)) valADC2 = (ADC_ChannelGetData(LPC_ADC, 1)&mascara)>>2;
	else if(ADC_ChannelGetStatus(LPC_ADC, 2, 1)) valADC3 = (ADC_ChannelGetData(LPC_ADC, 2)&mascara)>>2;
	else if(ADC_ChannelGetStatus(LPC_ADC, 3, 1)) valADC4 = (ADC_ChannelGetData(LPC_ADC, 3)&mascara)>>2;

	dato = (valADC1+valADC2+valADC3+valADC4);
	peso = dato*resolucion - (dato/25)*3 - tara;
	convert(peso,PESO);

	/*if(estaPesando()) {
		monto=getPrecio()*peso;
		convert(monto, MONTO);
	}
	*/
	return;
}

/*Rutina de servicio de interrupcion de TIMER0
 * Param:
 * 			NONE
 */
void TIMER0_IRQHandler(void){
	static uint8_t channel = 0;

	ADC_ChannelCmd(LPC_ADC, 0, DISABLE);
	ADC_ChannelCmd(LPC_ADC, 1, DISABLE);
	ADC_ChannelCmd(LPC_ADC, 2, DISABLE);
	ADC_ChannelCmd(LPC_ADC, 3, DISABLE);

	ADC_ChannelCmd(LPC_ADC, channel, ENABLE);

	ADC_StartCmd(LPC_ADC, ADC_START_NOW);

	channel++;

	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
	return;
}
