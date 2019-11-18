#include <logica.h>
#include <Multiplex.h>
#include <GPIO.h>
#include <teclado.h>
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"

void initTeclado(void);
void lecturaYAccion();
void lectura();
void chequeaFila(uint8_t col);
void setTeclaPulsada(uint8_t valor);
uint8_t getTeclaPulsada(void);
void confTimer(uint32_t time);
void EINT3_IRQHandler(void);
void TIMER1_IRQHandler(void);
void GPIO(void);

static uint8_t tecla;
static uint8_t teclaPulsada;
static uint8_t fila;
static uint8_t columna;
static uint8_t teclado[4][4]={
		{ 7, 8 , 9 , 10},  //10=A
		{ 4, 5 , 6 , 11},  //11=B
		{ 1, 2 , 3 , 12},  //12=C
		{15, 0 ,14 , 13}   //13=D, 14=*, 15=#
};

/*
 * Retorna el valor de teclaPulsada
 */
uint8_t getTeclaPulsada(void){
	return teclaPulsada;
}

/*
 * Should be SI or NO
 */
void setTeclaPulsada(uint8_t valor){
	teclaPulsada=valor;
}

/*
 * Inicializa el teclado, utiliza EINT
 * TODO Ver el tipo de resistencias
 */
void initTeclado(void){
	GPIO();
//	confPinSal(2,0,3); 				//Configura como salida P2.0-P2.3
//	confPinSoC(2,0,3,0);
	GPIO_ClearValue(2,0xF);
	GPIO_ClearInt(2,(0b1111<<4));
	GPIO_IntCmd(2,(0b1111<<4),1);	//Habilita interrupciones P2.4-P0.7
	EXTI_ClearEXTIFlag(EXTI_EINT3);
	confTimer(150000);
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void GPIO(void){
	PINSEL_CFG_Type config_pinsel;
		config_pinsel.Portnum=2;
		config_pinsel.Pinnum=0;
		config_pinsel.Funcnum=0;
		config_pinsel.Pinmode=PINSEL_PINMODE_PULLUP;
		config_pinsel.OpenDrain = PINSEL_PINMODE_NORMAL;
		PINSEL_ConfigPin(&config_pinsel);
		config_pinsel.Pinnum=1;
		PINSEL_ConfigPin(&config_pinsel);
		config_pinsel.Pinnum=2;
		PINSEL_ConfigPin(&config_pinsel);
		config_pinsel.Pinnum=3;
		PINSEL_ConfigPin(&config_pinsel);
		config_pinsel.Pinnum=4;
		PINSEL_ConfigPin(&config_pinsel);
		config_pinsel.Pinnum=5;
		PINSEL_ConfigPin(&config_pinsel);
		config_pinsel.Pinnum=6;
		PINSEL_ConfigPin(&config_pinsel);
		config_pinsel.Pinnum=7;
		PINSEL_ConfigPin(&config_pinsel);


		GPIO_SetDir(2,(0xF<<0),1);	//P0.15 A P0.18 SALIDAS
		GPIO_SetDir(2,(0xF<<4),0);	//P0.23 A P0.26 ENTRADAS

//		GPIO_ClearInt(2, (0xF<<23));	//limpio interrupciones de puertos
//		GPIO_IntCmd(2,(0xF<<23),Edge); //habilita interrupciones p0.23 a p0.26

//		EXTI_ClearEXTIFlag(EXTI_EINT3);
//		NVIC_EnableIRQ(EINT3_IRQn);			//habilito interrupcion por TECLADO
}

/*
 * Configura los 4 pines de interrupción externa.
 */
void EINT3_IRQHandler(void){
	teclaPulsada=SI;
	TIM_ResetCounter(LPC_TIM1);
	//NVIC_EnableIRQ(TIMER1_IRQn); //TODO determinar forma de interrupcion

	lecturaYAccion();
	for(uint32_t i=0;i<100000;i++){}
	//EXTI_ClearEXTIFlag(EXTI_EINT3);
	//GPIO_ClearInt(2, (0b1111)<<4);
	//NVIC_DisableIRQ(EINT3_IRQn); //Deshabilito la interrupcion por GPIO
	return;
}

void lecturaYAccion(){
	lectura();
	accion(tecla);
	NVIC_DisableIRQ(TIMER1_IRQn);
	EXTI_ClearEXTIFlag(EXTI_EINT3);
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void lectura(){
	if(GPIO_GetIntStatus(2,4,1)==ENABLE){
		columna=0;
		chequeaFila(0);
	}
	else if(GPIO_GetIntStatus(2,5,1)==ENABLE){
		columna=1;
		chequeaFila(1);
	}
	else if(GPIO_GetIntStatus(2,6,1)==ENABLE){
		columna=2;
		chequeaFila(2);
	}
	else if(GPIO_GetIntStatus(2,7,1)==ENABLE){
		columna=3;
		chequeaFila(3);
	}
	GPIO_ClearInt(2,(0b1111<<4));
	tecla=teclado[fila][columna];
	confPinSoC(2,0,3,0);
	return;
}

void chequeaFila(uint8_t col){
	GPIO_SetValue(2,0b1110);								//Seteo P2.1-2.3
	GPIO_ClearValue(2,1);
	if((LPC_GPIO2->FIOPIN&(1<<(col+4)))!=1){fila=0;return;}	//Si tengo un 0 en la entrada, la fila es la 0
	LPC_GPIO2->FIOCLR|=(1<<1);
	LPC_GPIO2->FIOSET|=(1<<0);							//Muevo el 0 a la fila 1
	if((LPC_GPIO2->FIOPIN&(1<<(col+4)))!=1){fila=1;return;}	//Si tengo un 0 en la entrada, la fila es la 1
	LPC_GPIO2->FIOCLR|=(1<<2);
	LPC_GPIO2->FIOSET|=(1<<1);							//Muevo el 0 a la fila 2
	if((LPC_GPIO2->FIOPIN&(1<<(col+4)))!=1){fila=2;return;}	//Si tengo un 0 en la entrada, la fila es la 2
	LPC_GPIO2->FIOCLR|=(1<<3);
	LPC_GPIO2->FIOSET|=(1<<2);							//Muevo el 0 a la fila 3
	if((LPC_GPIO2->FIOPIN&(1<<(col+4)))!=1){fila=3;return;}	//Si tengo un 0 en la entrada, la fila es la 3

}
//--------------------------------------------------------------------------------------------------------------------------------------
void confTimer(uint32_t time){
		TIM_TIMERCFG_Type TIMcfg;
		TIMcfg.PrescaleOption = TIM_PRESCALE_USVAL;
		TIMcfg.PrescaleValue = 1;
		TIM_Init(LPC_TIM1,TIM_TIMER_MODE ,&TIMcfg);

		TIM_MATCHCFG_Type MATCHcfg;
		MATCHcfg.MatchChannel = 0;
		MATCHcfg.IntOnMatch = ENABLE;
		MATCHcfg.StopOnMatch = DISABLE;
		MATCHcfg.ResetOnMatch = ENABLE;
		MATCHcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
		MATCHcfg.MatchValue = time;
		TIM_ConfigMatch(LPC_TIM1, &MATCHcfg);

		//NVIC_EnableIRQ(TIMER0_IRQn);
		return;

}
void TIMER1_IRQHandler(void){
	lecturaYAccion();
	return;
}
