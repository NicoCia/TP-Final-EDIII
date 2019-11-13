/*
 * logica.c
 *
 *  Created on: 10 de nov. de 2019
 *      Author: nico-
 */
#include "LPC17xx.h"
#include "teclado.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "Multiplex.h"
#include "Pesaje.h"


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
typedef enum{MANUAL=10,PRECONF,CARGAR_VALORES,ENTER,CANCELAR,TARA}TECLAS;//TODO Relacionarlo con el teclado
/*
#define ENTER			(uint8_t) 0
#define CANCELAR		(uint8_t) 0
#define TARA			(uint8_t) 0
*/
//TODO Borrar lo que sigue
//typedef enum{CANTIDAD=2,MONTO=3};
/*
#define CODIGO			(uint8_t) 0		//TODO ver correspondencias
#define PRECIO			(uint8_t) 1
#define CANTIDAD		(uint8_t) 2
#define MONTO			(uint8_t) 3
*/
static uint16_t datos[100][3];
static uint16_t compra[100][4];
static uint8_t modo=MODO_PESANDO;
static uint8_t contaDig=0;
static uint16_t precio=0;
static uint8_t codigo=0;
static uint8_t sumaParcial=0;
static uint8_t cantCompras=0;
static uint8_t tipoCompra=0;
static uint8_t vendido=0;
static uint8_t estado=0;
static uint8_t codigoAMod=0;
static uint8_t precioAMod=0;
static uint8_t cantidadAMod=0;
static uint8_t contaModDatos=0;

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
void reiniciarCuenta();

void initTabla(){
	for(uint8_t i=0;i<100;i++){
		for(uint8_t j=0;j<3;j++){
			datos[i][j]=0;
		}
	}
	reempezarCompra();
	vendido=0;
	cantCompras=0;
}

void cargarTabla(uint16_t datosACargar[100][3]){
	for(uint8_t i=0;i<100;i++){
		for(uint8_t j=0;j<3;j++){
			datos[i][j]=datosACargar[i][j];
		}
	}
	return;
}

uint16_t getPrecio(){
	return precio;
}

void cancelarUltimaCompra(){			//Cancela la última compra
	//TODO
}

void accion(uint8_t tecla){
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
		case CARGAR_VALORES:modo=MODO_MODIFICAR_DATOS;												break;
		default:																					break;
	}
	return;
}

void agregarCompra(){
	if		(tipoCompra==MANUAL)	compra[cantCompras][0]=100;			//Si es manual, pongo el campo 0 en 0
	else if	(tipoCompra==PRECONF)	compra[cantCompras][0]=codigo;		//Si es preconf, pongo el codigo
	compra[cantCompras][PRECIO]=	precio;								//Pongo el precio
	compra[cantCompras][CANTIDAD]=	getPeso();							//Peso
	compra[cantCompras][MONTO]=		precio*getPeso();					//Monto
	sumaParcial+=precio*getPeso();											//Agrego a la suma parcial
	cantCompras++;														//Incremento el contador de productos comprados
	return;
}

uint8_t estaPesando(){
	return modo==MODO_PESANDO;
}
//-----------------------------------------------  MODO SUMA PARCIAL -------------------------------------------------------------------
void modoSumaParcial(uint8_t tecla){
	switch(tecla){
		case ENTER:			/*modo=MODO_FINAL;*/	   finalizarCompra();							break;	//TODO cancelar la última compra;convert(sumaParcial,MONTO);<-Eso taba antes
		case CANCELAR:		/*modo=MODO_FINAL;*/ 	   reiniciarCompra();							break;
		case PRECONF:		modo=MODO_PRECONFIGURADO;  contaDig=0;precio=0;convert(precio,PRECIO);	break;
		case MANUAL:		modo=MODO_MANUAL;		   contaDig=0;precio=0;convert(precio,PRECIO);	break;
		default:																					break;
	}
	return;
}
//-----------------------------------------------  MODO    MANUAL -------------------------------------------------------------------
void modoManual(uint8_t tecla){
	switch(tecla){
		case ENTER:			modo=MODO_PESANDO;			contaDig=0;									break;
		case CANCELAR:		modo=MODO_PESANDO;			contaDig=0;precio=0;convert(precio,PRECIO);	break; 	//TODO ver como organizar el cancelar de acá, si borra ultimo digito o no
		case TARA:																							//TODO considerar el resto de las teclas
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
		case TARA:																		//TODO considerar el resto de las teclas
		case MANUAL:
		case PRECONF:
		case CARGAR_VALORES:													break;
		default:										agregarNumPC(tecla);	break;
	}
	return;
}

void agregarNumPC(uint8_t tecla){			//TODO considerar de apretar el ENTER, plus, como borrar y/o cancelar un numero o todo el numero, como está apenas se setea el numero 2, se pasa a seteando.
//	if(contaDig==0)	codigo=0;	TODO ver si se puede comentar esta linea asi como la comente :P
	codigo=codigo*10+tecla;
	convert(codigo,MONTO);					//Muestra el código en el tercer display
	contaDig++;
	if(contaDig==2){cargarPreConf();}
	return;
}

void cargarPreConf(){
	if(codigo!=0){
		modo=MODO_PESANDO;
		precio=datos[codigo][PRECIO];
		convert(precio,PRECIO);
		tipoCompra=PRECONF;
		contaDig=0;
	}
	return;
}
//-----------------------------------------------  MODO    FINAL -------------------------------------------------------------------
/*
void modoFinal(uint8_t tecla){
	switch(tecla){
		case CANCELAR:		modo=SUMA_PARCIAL;								break;
		case ENTER:			modo=MODO_PESANDO;		finalizarCompra();		break;
		default:			break;
	}
	return;
}
*/
/*
 * Resta del stock, si hay disponible. En el caso de que no, por algun error humano al cargar los datos, se pone en 0, ya que de lo
 * contrario, al ser unsigned int, se cargaria con un valor elevado.
 * Incrementa la variable de lo vendido hasta el momento
 * Reinicia las variables
 */

void finalizarCompra(){
	for(uint8_t i=0;i<cantCompras;i++){
		if(compra[i][0]!=100){
			if(datos[compra[i][0]][1]>compra[i][CANTIDAD])	datos[compra[i][0]][1]=0;
			else 											datos[compra[i][0]][1]-=compra[i][CANTIDAD];
		}
	}
	vendido+=sumaParcial;
	reiniciarCompra();
	return;
}

void reiniciarCompra(){							//TODO puede haber un botón que haga esto.
	sumaParcial=0;
	cantCompras=0;
	reempezarCompra();
	modo=MODO_PESANDO;
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
		case CANCELAR: 					modo=MODO_PESANDO;		reiniciarCuenta();		break;
		case ENTER:		if(estado==0)	modo=MODO_PESANDO;		else	estado++;		break;
		case TARA:																		//TODO considerar el resto de las teclas
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
	if(estado==1){
		codigoAMod=codigoAMod*10+tecla;
		contaModDatos++;
		convert(codigoAMod,PRECIO);				//Muestro el código en el primerDisplay
		if(contaModDatos==2)estado++;
	}
	else if	(estado==2){
		precioAMod=precioAMod*10+tecla;
		contaModDatos++;
		convert(precioAMod,CANTIDAD);			//Muestro la cantidad en el segundoDisplay
		if(contaModDatos==7)estado++;
	}
	else if	(estado==3){
		cantidadAMod=cantidadAMod*10+tecla;
		contaModDatos++;
		convert(cantidadAMod,MONTO);			//Muestro la cantidad en el tercerDisplay
		if(contaModDatos==13)estado++;

	}

	else if(estado==4){
		if(codigoAMod!=0)	datos[codigoAMod][0]=codigoAMod;
		if(precioAMod!=0)	datos[codigoAMod][1]=precioAMod;
		if(cantidadAMod!=0)	datos[codigoAMod][2]=cantidadAMod;
		reiniciarCuenta();
	}

	return;
}

void reiniciarCuenta(){
	estado=0;
	codigoAMod=0;
	precioAMod=0;
	cantidadAMod=0;
	return;
}
//---------------------------------------------------------------------------------------------------------------------------
