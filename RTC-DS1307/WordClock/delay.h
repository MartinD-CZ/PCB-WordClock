#ifndef _DELAY_H
#define _DELAY_H

#define F_CPU 48000000UL

void delay_ms(uint16_t ms);
void delay_us(uint16_t us);
void delay_init();

#endif
