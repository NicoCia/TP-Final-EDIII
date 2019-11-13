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
	confPinSal(2,0,3); 				//Configura como salida P2.0-P2.3
	GPIO_IntCmd(2,(1111<<4),0);		//Habilita interrupciones P2.4-P0.7
	GPIO_ClearInt(2,(0b1111<<3));
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

/*
 * Configura los 4 pines de interrupción externa.
 */
void EINT3_IRQHandler(void){
	teclaPulsada=SI;
	NVIC_EnableIRQ(TIMER0_IRQn); //TODO determinar forma de interrupcion
	NVIC_DisableIRQ(EINT3_IRQn); //Deshabilito la interrupcion por GPIO
	return;
}

void lecturaYAccion(){
	lectura();
	accion(tecla);
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void lectura(){
	if(GPIO_GetIntStatus(2,4,0)){
		columna=0;
		chequeaFila(0);
	}
	else if(GPIO_GetIntStatus(2,5,0)){
		columna=1;
		chequeaFila(1);
	}
	else if(GPIO_GetIntStatus(2,6,0)){
		columna=2;
		chequeaFila(2);
	}
	else if(GPIO_GetIntStatus(2,7,0)){
		columna=3;
		chequeaFila(3);
	}
	GPIO_ClearInt(2,(0b1111<<3));
	tecla=teclado[fila][columna];
	return;
}

void chequeaFila(uint8_t col){
	confPinSoC(2,1,3,1);								//Seteo P2.1-2.3
	if((LPC_GPIO2->FIOPIN&(1<<col))!=1){fila=0;return;}	//Si tengo un 0 en la entrada, la fila es la 0
	LPC_GPIO2->FIOCLR|=(1<<1);
	LPC_GPIO2->FIOSET|=(1<<0);							//Muevo el 0 a la fila 1
	if((LPC_GPIO2->FIOPIN&(1<<col))!=1){fila=1;return;}	//Si tengo un 0 en la entrada, la fila es la 1
	LPC_GPIO2->FIOCLR|=(1<<2);
	LPC_GPIO2->FIOSET|=(1<<1);							//Muevo el 0 a la fila 2
	if((LPC_GPIO2->FIOPIN&(1<<col))!=1){fila=2;return;}	//Si tengo un 0 en la entrada, la fila es la 2
	LPC_GPIO2->FIOCLR|=(1<<3);
	LPC_GPIO2->FIOSET|=(1<<2);							//Muevo el 0 a la fila 3
	if((LPC_GPIO2->FIOPIN&(1<<col))!=1){fila=3;return;}	//Si tengo un 0 en la entrada, la fila es la 3

}
