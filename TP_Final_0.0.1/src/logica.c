/*
 * logica.c
 *
 *  Created on: 10 de nov. de 2019
 *      Author: nico-
 */
#include <Multiplex.h>
#include <Pesaje.h>
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_uart.h"


typedef enum{MODO_PESANDO=0,MODO_MANUAL,MODO_PRECONFIGURADO,SUMA_PARCIAL,MODO_MODIFICAR_DATOS}MODOS;
/*#define MODO_MANUAL		(uint8_t) 0
#define MODO_PRECONF	(uint8_t) 0
#define MODO_PESANDO	(uint8_t) 0
#define	SUMA_PARCIAL	(uint8_t) 0
#define	MODO_FINAL		(uint8_t) 0*/
//typedef enum{MANUAL=0,PRECONF}TIPO_DE_PESADO;
/*#define MANUAL			(uint8_t) 0
#define PRECONF			(uint8_t) 0
*/
typedef enum{MANUAL=10,PRECONF,CARGAR_VALORES,ENTER,CANCELAR,TARA}TECLAS;
typedef enum{CODIGO=0};

static uint16_t datos[100][3];
static uint16_t compra[100][4];
static uint8_t modo=MODO_PESANDO;
static uint8_t contaDig=0;
static uint16_t precio=0;
static uint8_t codigo=0;
static uint32_t sumaParcial=0;
static uint8_t cantCompras=0;
static uint8_t tipoCompra=MANUAL;
static uint8_t vendido=0;
static uint8_t estado=0;
static uint32_t codigoAMod=0;
static uint32_t precioAMod=0;
static uint32_t cantidadAMod=0;
static uint32_t contaModDatos=0;

void initTabla();
void cargarTabla(uint16_t datosACargar[100][3]);
uint16_t getPrecio();
void accion(uint8_t tecla);
void modoPesando(uint8_t tecla);
void agregarCompra();
uint8_t estaPesando();
void modoSumaParcial(uint8_t tecla);
void modoManual(uint8_t tecla);
void agregarNumManual(uint8_t tecla);
void modoPreConfigurado(uint8_t tecla);
void agregarNumPC(uint8_t tecla);
void cargarPreConf();
void finalizarCompra();
void reiniciarCompra();
void reempezarCompra();
void modoModificarDatos(uint8_t tecla);
void modificarDatos(uint8_t tecla);
void reiniciarCuentaYSalir();
void cargarNuevoDato();
void terminarCarga();
void getStockEnAscii(uint8_t datosEnChar[]);
void agregarDaC(uint8_t *datosEnChar,uint16_t datoNumerico,uint8_t *contaChar);
void pruebaDemo();
uint8_t getModo();
uint8_t pasarAAscii(uint16_t num);


void initTabla(){						//Inicia la tabla de datos en 0

	for(uint8_t i=0;i<100;i++){
		datos[i][0]=100;
		for(uint8_t j=1;j<3;j++){
			datos[i][j]=0;
		}
	}
	reempezarCompra();
	vendido=0;
	cantCompras=0;
}

uint8_t getModo(){						//Retorna el modo actual
	return modo;
}

uint16_t getPrecio(){					//Retorna el precio de lo que se pesa en el momento
	return precio;
}

void accion(uint8_t tecla){				//Me lleva al modo en el que me encuentro
	switch(modo){
			case MODO_PESANDO: 			modoPesando(tecla);			break;
			case MODO_MANUAL:			modoManual(tecla);			break;
			case MODO_PRECONFIGURADO:	modoPreConfigurado(tecla);	break;
			case SUMA_PARCIAL: 			modoSumaParcial(tecla);		break;
			case MODO_MODIFICAR_DATOS: 	modoModificarDatos(tecla);	break;
			default: break;
	}
	return;
}
//-----------------------------------------------  MODO    PESANDO -------------------------------------------------------------------
void modoPesando(uint8_t tecla){
	switch(tecla){
		case ENTER: 		modo=SUMA_PARCIAL;		agregarCompra();	convert(sumaParcial,MONTO);	break;
		case CANCELAR:		modo=SUMA_PARCIAL;							convert(sumaParcial,MONTO);	break;
		case TARA:			tarar();																break;
		case CARGAR_VALORES:modo=MODO_MODIFICAR_DATOS;					convert(0,PESO);			break;
		default:																					break;
	}
	return;
}

void agregarCompra(){
	if		(tipoCompra==MANUAL)	compra[cantCompras][0]=100;			//Si es manual, pongo el campo 0 en 0
	else if	(tipoCompra==PRECONF)	compra[cantCompras][0]=codigo;		//Si es preconf, pongo el codigo
	compra[cantCompras][PRECIO]=	precio;								//Pongo el precio
	compra[cantCompras][CANTIDAD]=	getPeso();							//Peso
	compra[cantCompras][MONTO]=		precio*getPeso()/1000;				//Monto
	sumaParcial+=precio*getPeso()/1000;									//Agrego a la suma parcial
	cantCompras++;														//Incremento el contador de productos comprados
	return;
}

uint8_t estaPesando(){
	return modo==MODO_PESANDO;
}

uint8_t estaModificando(){
	return modo==MODO_MODIFICAR_DATOS;
}
//-----------------------------------------------  MODO SUMA PARCIAL -------------------------------------------------------------------
void modoSumaParcial(uint8_t tecla){
	switch(tecla){
		case ENTER:									   finalizarCompra();										break;
		case CANCELAR:							 	   reiniciarCompra();										break;
		case PRECONF:		modo=MODO_PRECONFIGURADO;  contaDig=0;precio=0;convert(precio,PRECIO);				break;
		case MANUAL:		modo=MODO_MANUAL;		   codigo=0; contaDig=0;precio=0;convert(precio,PRECIO);	break;
		default:																								break;
	}
	return;
}
//-----------------------------------------------  MODO    MANUAL -------------------------------------------------------------------
void modoManual(uint8_t tecla){
	switch(tecla){
		case ENTER:			modo=MODO_PESANDO;			contaDig=0;									break;
		case CANCELAR:		modo=MODO_PESANDO;			contaDig=0;precio=0;convert(precio,PRECIO);	break;
		case TARA:
		case MANUAL:
		case PRECONF:
		case CARGAR_VALORES:																		break;
		default:										agregarNumManual(tecla);					break;
	}
	return;
}

void agregarNumManual(uint8_t tecla){
	if(contaDig==0)	precio=0;									//Si estoy poniendo el primer dígito, pongo precio en 0
	precio=precio*10+tecla;										//Agrego dígitos
	convert(precio,PRECIO);										//Muestro en el display
	contaDig++;													//Aumento el contador
	if(contaDig==5){		modo=MODO_PESANDO;	contaDig=0;	   }//Si ya escribi los 5 numeros, cambio de modo
	return;
}
//-----------------------------------------------  MODO  PRE CONFIGURADO -------------------------------------------------------------------
void modoPreConfigurado(uint8_t tecla){
	switch(tecla){
		case ENTER:			modo=MODO_PESANDO;			cargarPreConf();		break;
		case CANCELAR:		modo=MODO_PESANDO;		/*cancelarUltimoDigito();*/	break;
		case TARA:
		case MANUAL:
		case PRECONF:
		case CARGAR_VALORES:													break;
		default:										agregarNumPC(tecla);	break;
	}
	return;
}

void agregarNumPC(uint8_t tecla){
	if(contaDig==0)	codigo=0;
	codigo=codigo*10+tecla;
	convert(codigo,MONTO);					//Muestra el código en el tercer display
	contaDig++;								//Incremento contador
	if(contaDig==2){cargarPreConf();}		//Si ya apreté 2 números, cargo el dato
	return;
}

void cargarPreConf(){						//Carga los valores
	if(codigo!=0){
		modo=MODO_PESANDO;					//Cambio a modo pesando
		precio=datos[codigo][PRECIO];		//Seteo el precio como el precio del producto seleccionado por código
		convert(precio,PRECIO);				//Muestro el precio en el display
		tipoCompra=PRECONF;					//Seteo el tipo de compra en Preconfigurado
		contaDig=0;							//Reseteo el contador
	}
	return;
}
/*
 * Resta del stock, si hay disponible. En el caso de que no, por algun error humano al cargar los datos, se pone en 0, ya que de lo
 * contrario, al ser unsigned int, se cargaria con un valor elevado.
 * Incrementa la variable de lo vendido hasta el momento
 * Reinicia las variables
 */

void finalizarCompra(){
	static uint8_t mensajeFaltante[]="\nHay menos de 5Kg del producto: ";
	uint8_t mensaje[2]="";
	uint8_t mensajeFinal[]="\n\n\r";
	uint8_t conta[1]={0};
	for(uint8_t i=0;i<cantCompras;i++){
		if(compra[i][0]!=100){
			if(datos[compra[i][0]][CANTIDAD]<compra[i][CANTIDAD])	datos[compra[i][0]][CANTIDAD]=0;
			else 													{datos[compra[i][0]][CANTIDAD]-=compra[i][CANTIDAD];}
			if(datos[compra[i][0]][CANTIDAD]<5000){
				UART_Send(LPC_UART0, mensajeFaltante, sizeof(mensajeFaltante), BLOCKING);	//Si me quedan menos de 5Kg de producto, envio mensaje de faltante.
				agregarDaC(mensaje,datos[compra[i][CODIGO]][CODIGO],conta);
				UART_Send(LPC_UART0, mensaje, sizeof(mensaje), BLOCKING);
				UART_Send(LPC_UART0, mensajeFinal, sizeof(mensajeFinal), BLOCKING);
																				}
		}
	}
	vendido+=sumaParcial;			//Variable que lleva cuenta de lo vendido desde que se inició la balanza
	reiniciarCompra();				//Reinicia la compra
	return;
}

void reiniciarCompra(){
	sumaParcial=0;					//Resetea contadores
	cantCompras=0;
	reempezarCompra();				//Limpia la matriz de compras
	modo=MODO_PESANDO;				//Pasa a modo pesando
	convert(sumaParcial,MONTO);		//Coloca el monto total en los displays correspondientes
	precio=0;						//Vuelve precio a 0
	convert(precio,PRECIO);			//Muestra 0 en precio
	return;
}


/*
 * Reinicia la matriz de compras realizadas hasta el momento
 */
void reempezarCompra(){
	for(uint8_t i=0;i<100;i++){
		for(uint8_t j=0;j<4;j++){
			compra[i][j]=0;
		}
	}
}
//------------------------------------------------------MODO MODIFICAR DATOS------------------------------------------------------------
void modoModificarDatos(uint8_t tecla){
	switch(tecla){
		case CANCELAR: 					modo=MODO_PESANDO;		reiniciarCuentaYSalir();		break;
		case ENTER:						contaModDatos=0;		estado++;	if(estado==3)terminarCarga();			break;
		case TARA:
		case MANUAL:
		case PRECONF:
		case CARGAR_VALORES:															break;
		default:												modificarDatos(tecla);	break;
	}
}
/*
 * Modifica los valores de la tabla
 */
void modificarDatos(uint8_t tecla){
	if(estado==0){
		codigoAMod=codigoAMod*10+tecla;
		contaModDatos++;
		convert(codigoAMod,PRECIO);				//Muestro el código en el primerDisplay
		if(contaModDatos==2){estado++;contaModDatos=0;}
	}
	else if	(estado==1){
		precioAMod=precioAMod*10+tecla;
		contaModDatos++;
		convert(precioAMod,CANTIDAD);			//Muestro el precio en el segundoDisplay
		if(contaModDatos==5){estado++;contaModDatos=0;}
	}
	else if	(estado==2){
		cantidadAMod=cantidadAMod*10+tecla;
		contaModDatos++;
		convert(cantidadAMod,MONTO);			//Muestro la cantidad en el tercerDisplay
		if(contaModDatos==5){estado++;terminarCarga();}

	}

	return;
}
void terminarCarga(){
	cargarNuevoDato();							//Reemplaza los valores anteriores por los nuevos
	reiniciarCuentaYSalir();					//Resetea contadores y vuelve a modo pesando
	return;
}
void cargarNuevoDato(){
	if(codigoAMod!=0)	datos[codigoAMod][CODIGO]=codigoAMod;			//Reemplaza los valores anteriores por los nuevos
	if(precioAMod!=0)	datos[codigoAMod][PRECIO]=precioAMod;
	if(cantidadAMod!=0)	datos[codigoAMod][CANTIDAD]=cantidadAMod;
	reiniciarCuentaYSalir();
	return;
}
void reiniciarCuentaYSalir(){					//Resetea contadores y vuelve a modo pesando
	estado=0;
	codigoAMod=0;
	precioAMod=0;
	cantidadAMod=0;
	convert(0,PRECIO);
	convert(0,CANTIDAD);
	convert(0,MONTO);
	modo=MODO_PESANDO;
	return;
}

//---------------------------------------------------------------------------------------------------------------------------

void getStockEnAscii(uint8_t datosEnChar[]){
	uint8_t contaChar[1]={0};

	for(uint8_t i=0;i<100;i++){										//Recorro la matriz
		if(datos[i][0]!=100){										//Si no hay nada cargado acá, sigo con el próximo valor.
			for(uint8_t j=0;j<3;j++){
				agregarDaC(datosEnChar,datos[i][j],contaChar);		//Al arreglo datosEnChar, le agrega en Ascii el número correspondiente
				datosEnChar[contaChar[0]]='\t';						//Agrega una tabulación
				contaChar[0]++;										//Incrementa el contador de la cantidad de valores que tiene el arreglo
			}
			datosEnChar[contaChar[0]]='\n';							//Agrega un salto de línea
			contaChar[0]++;											//Incrementa el contador
			datosEnChar[contaChar[0]]='\r';							//Agrega un retroceso
			contaChar[0]++;											//Incrementa el contador
		}
	}
	datosEnChar[contaChar[0]]='z';									//Carracter de final

	return;
}


void agregarDaC(uint8_t *datosEnChar,uint16_t datoNumerico,uint8_t *contaChar){

		uint8_t aux[8];
		uint8_t conta=0;
		while(datoNumerico>9){						//Descompone el número en dígitos
			aux[conta]=datoNumerico%10;
			datoNumerico=datoNumerico/10;
			conta++;
		}
		aux[conta]=datoNumerico;
		conta++;

		for(uint8_t i=1;i<conta+1;i++){										//Carga los dígitos en Ascii, lo recorre al revés porque la forma
			datosEnChar[contaChar[0]]=pasarAAscii(aux[conta-i]);			//de descomponer al número hace que queden al revés.
			contaChar[0]++;
		}

	return;
}

uint8_t pasarAAscii(uint16_t num){				//Carga el valor en Ascii del digito tipo valor uint16_t pasado por parametro
	switch(num){
		case 0:	return '0';
		case 1: return '1';
		case 2: return '2';
		case 3: return '3';
		case 4: return '4';
		case 5: return '5';
		case 6: return '6';
		case 7: return '7';
		case 8: return '8';
		case 9: return '9';
		default: return ' ';
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
//Carga la tabla con algunos valores
void pruebaDemo(){
	datos[5][CODIGO]=5;
	datos[5][CANTIDAD]=12000;							//Cantidad en gramos
	datos[5][PRECIO]=2500;								//Precio en centavos

	datos[39][CODIGO]=39;
	datos[39][CANTIDAD]=12000;							//Cantidad en gramos
	datos[39][PRECIO]=4200;								//Precio en centavos

	datos[10][CODIGO]=10;
	datos[10][CANTIDAD]=50000;							//Cantidad en gramos
	datos[10][PRECIO]=4700;								//Precio en centavos

	datos[26][CODIGO]=26;
	datos[26][CANTIDAD]=42000;							//Cantidad en gramos
	datos[26][PRECIO]=37500;							//Precio en centavos

	datos[25][CODIGO]=25;
	datos[25][CANTIDAD]=22000;							//Cantidad en gramos
	datos[25][PRECIO]=2500;								//Precio en centavos

	datos[13][CODIGO]=13;
	datos[13][CANTIDAD]=33000;							//Cantidad en gramos
	datos[13][PRECIO]=3900;								//Precio en centavos

	return;

}
