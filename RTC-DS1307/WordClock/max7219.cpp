#include "stm32f0xx.h"
#include "max7219.h"
#include "delay.h"

#define DEVICES			3

#define REG_NOOP		0x0
#define REG_DIG0		0x1
#define REG_DIG1		0x2
#define REG_DIG2		0x3
#define REG_DIG3		0x4
#define REG_DIG4		0x5
#define REG_DIG5		0x6
#define REG_DIG6		0x7
#define REG_DIG7		0x8
#define REG_DECODE	0x9
#define REG_INTEN		0xA
#define REG_SCAN		0xB
#define REG_SDOWN		0xC
#define REG_TEST		0xF


void max7219_init(void)
{
	//enable clock
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	//configure SPI (16 bits data size)
	SPI1->CR2 |= SPI_CR2_SSOE | SPI_CR2_DS;
	SPI1->CR1 |= SPI_CR1_SPE | SPI_CR1_BR_1 | SPI_CR1_MSTR | SPI_CR1_SSM;

	//configure GPIO for SPI
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR7;
	//AFIO not needed
	
	//configure CS pin
	GPIOA->MODER |= GPIO_MODER_MODER6_0;
	CS_HIGH;
	
	delay_ms(10);
	
	//set default values
	max7219_writeAllRegisters(REG_SDOWN, 0x01);				//power up
	delay_us(1);
	max7219_setIntensity(0x0F);												//lowest intensity
	delay_us(1);
	max7219_zeroDisplay();														//zero anything left in the memory
	delay_us(1);
	max7219_writeAllRegisters(REG_SCAN, 0x07);				//enable all digits
	delay_us(1);
	max7219_writeAllRegisters(REG_TEST, 0x01);				//LED test
	delay_ms(250);	
	max7219_writeAllRegisters(REG_TEST, 0x00);
}

void max7219_writeSingleRegister(uint8_t reg, uint8_t data)
{
	uint16_t out = ((reg & 0xF) << 8) | data;
	SPI1->DR = out;
	while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
	delay_us(1);
}

void max7219_writeAllRegisters(uint8_t reg, uint8_t data)
{
	CS_LOW;
	for (uint8_t i = 0; i < DEVICES; i++) max7219_writeSingleRegister(reg, data);
	CS_HIGH;
}

void max7219_setIntensity(uint8_t intensity)			//from 0x00 to 0x0F (ie 15)
{
	if (intensity > 0x0F) intensity = 0x0F;
	
	max7219_writeAllRegisters(REG_INTEN, intensity);
}

void max7219_zeroDisplay(void)
{
	for (uint8_t i = 0; i < 8; i++)					//zero display
	{
		max7219_writeAllRegisters(REG_DIG0 + i, 0x00);
		delay_us(10);
	}
}

void max7219_setBit(uint8_t device, uint8_t digit, uint8_t bit)
{
	int8_t i = DEVICES - 1;
	
	CS_LOW;
	for (; i > device; i--) max7219_writeSingleRegister(REG_NOOP, 0x00);
	max7219_writeSingleRegister(REG_DIG0 + digit, 0x01 << bit);
	i--;
	for (; i >= 0; i--) max7219_writeSingleRegister(REG_NOOP, 0x00);
	CS_HIGH;
}
