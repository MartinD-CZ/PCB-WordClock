#ifndef _UART_H
#define _UART_H

void uart_init(uint32_t baudrate);
void uart_print(char *msg);
void uart_printf(char *szFormat, ...);

#endif
