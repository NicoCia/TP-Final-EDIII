#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "Multiplex.h"

void confEINT(void);
void confTIM(void);
void confADC(void);
void confUART(void);
void confGPIO(void);

void confADCPin(uint8_t num);
void confUARTPin(void);
void confEINTPin(uint8_t num);

void ADC_IRQHanler(void);

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
/*Configuracion Timer0 para producir un Match cada 250 ms
 * Param:
 * 			NONE
 */
void confTIM(void){
	TIM_TIMERCFG_Type TIMcfg;
	TIMcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	TIMcfg.PrescaleValue = 1;
	TIM_Init(LPC_TIM0,TIM_TIMER_MODE ,&TIMcfg);

	TIM_MATCHCFG_Type MATCHcfg;
	MATCHcfg.MatchChannel = 1;
	MATCHcfg.IntOnMatch = DISABLE;
	MATCHcfg.StopOnMatch = DISABLE;
	MATCHcfg.ResetOnMatch = ENABLE;
	MATCHcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	MATCHcfg.MatchValue = 250000;
	TIM_ConfigMatch(LPC_TIM0, &MATCHcfg);

	return;
}

/*Configuracion ADC para tomar muestras cada vez que se producza un Match en Timer0
 * Param:
 * 			NONE
 */
void confADC(void){
	confADCPin_0a3(0);
	ADC_Init(LPC_ADC, 200000);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_ChannelCmd(LPC_ADC, 0, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);

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

	if(ADC_ChannelGetStatus(LPC_ADC, 0, 1)) valADC1 = ADC_ChannelGetData(LPC_ADC, 0);
	if(ADC_ChannelGetStatus(LPC_ADC, 1, 1)) valADC1 = ADC_ChannelGetData(LPC_ADC, 1);
	if(ADC_ChannelGetStatus(LPC_ADC, 2, 1)) valADC1 = ADC_ChannelGetData(LPC_ADC, 2);
	if(ADC_ChannelGetStatus(LPC_ADC, 3, 1)) valADC1 = ADC_ChannelGetData(LPC_ADC, 3);

	dato = valADC1+valADC2+valADC3+valADC4;

	convert(dato,PESO);

	return;
}
