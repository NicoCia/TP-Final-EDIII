#ifndef MULTIPLEX_H_
#define MULTIPLEX_H_

typedef enum {NO = 0, SI = !NO} Decimal;
typedef enum {PRECIO = 0, PESO = 5, VALOR = 10, TECLADO = 10} Bloque;

extern void convert(uint16_t dato, Bloque i);
extern void display(uint8_t *nums[], uint32_t disp, uint8_t dig);
extern void confSYSTICK(uint32_t time);

#endif /* MULTIPLEX_H_ */
