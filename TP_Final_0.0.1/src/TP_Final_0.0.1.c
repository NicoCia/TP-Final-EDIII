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


	confADC();
	confGPIO();
	confUART(); 
	initTabla();
  	pruebaDemo();
  	confSYSTICK(2);

  	while(1) {}

    return 0 ;
}
