TP DIGITAL III:

ADC:
	Controlado por Timer
	Si tiene pin que se pone en 1 al sentir peso, el timer se inicia por dicho pin, gpio: flanco subida cuenta, flanco bajada no
	Muestrea cada 200 ms cada canal. Timer interrumpe cada 50ms y dispara el canal correspondiente.


Multiplexación de display
	9-10-15 displays
	El tiempo se ve, SysTick.	
	
Teclado Matricial:
	Numeros son números
	A: Manual
	B: Código
	C: Cargar datos
	D: Enter (Suma al apretar una vez y termina al apretar 2 veces).
	*: Cancelar
	#: Tarar

Modos:
	Manual
	Valores ya cargados  
	Modificar valores

Funciones:
	Tomar lectura
		
GPIO:
	-ADC					P0.23-26			(4 pines)
	-Teclado				P2.0-7				(8 pines)
	-Displays				P0.4-11				(8 pines)	
	-Multiplicación de displays		P0.0-1				(2 pines)
						P0.15-18			(4 pines)
						P0.27-31			(5 pines)
						P2.8-13				(6 pines)
	-UART					P0.2-3				(2 pines)
	

