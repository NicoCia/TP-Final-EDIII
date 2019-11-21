#include <logica.h>
#include <Multiplex.h>
#include <Pesaje.h>
#include <Transmision.h>
#include <GPIO.h>
#include <teclado.h>
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpio.h"

int main(void) {


	confADC();
  	//confSYSTICK(10);
	confUART(); //TODO esta cambiar codigo de error
	initTabla();
  	pruebaDemo();
	//initTeclado();
	//confGPIO();
  	while(1) {}

    return 0 ;
}
