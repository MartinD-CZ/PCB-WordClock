#ifndef _MAX_H
#define _MAX_H

#define CS_LOW   		GPIOA->BSRR = GPIO_BSRR_BR_6
#define CS_HIGH  		GPIOA->BSRR = GPIO_BSRR_BS_6

void max7219_init(void);
void max7219_writeSingleRegister(uint8_t reg, uint8_t data);
void max7219_writeAllRegisters(uint8_t reg, uint8_t data);
void max7219_setIntensity(uint8_t intensity);
void max7219_zeroDisplay(void);
void max7219_setBit(uint8_t device, uint8_t digit, uint8_t bit);

#endif
