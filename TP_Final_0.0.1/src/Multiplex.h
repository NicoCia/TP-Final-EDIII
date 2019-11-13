#ifndef MULTIPLEX_H_
#define MULTIPLEX_H_

typedef enum {NO = 0, SI} Decimal;
typedef enum {PRECIO = 1, PESO, CANTIDAD = 2, MONTO, TECLADO} Bloque;

extern void convert(uint16_t dato, Bloque i);
extern void display(uint32_t disp, uint8_t dig);
extern void confSYSTICK(uint32_t time);

#endif /* MULTIPLEX_H_ */
