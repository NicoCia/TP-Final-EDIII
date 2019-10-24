#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"

void confEINT(void);
void confTIMER(void);
void confADC(void);
void confUART(void);
void confGPIO(void);

void confADCPin(uint8_t num);
void confUARTPin(void);
void confEINTPin(uint8_t num);







int main(void) {


	while(1) {
        i++ ;
    }
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
