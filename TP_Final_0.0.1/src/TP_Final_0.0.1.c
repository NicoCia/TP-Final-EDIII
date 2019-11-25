#include <logica.h>
#include <Multiplex.h>
#include <Pesaje.h>
#include <Transmision.h>
#include <GPIO.h>
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpio.h"

int main(void) {


	confADC();		//Configura el ADC
	confGPIO();		//Configura GPIO
	confUART(); 		//Configura el UART
	initTabla();		//Inicializa la tabla de logica
  	pruebaDemo();		//Carga datos en la tabla de logica
  	confSYSTICK(2);		//Configura el SysTick

  	while(1) {}

    return 0 ;
}
