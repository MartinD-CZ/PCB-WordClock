#include "stm32f0xx.h"
#include "spi.h"
//#include "config.h"

void spi_init(void)
{
	//enable clock
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	//configure SPI
	SPI1->CR2 |= SPI_CR2_SSOE;
	SPI1->CR1 |= SPI_CR1_SPE | SPI_CR1_BR_1 | SPI_CR1_MSTR | SPI_CR1_SSM;

	//configure GPIO
	GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7;
	//AFIO not needed
}

uint8_t spi_sendByte(uint8_t data)
{
	*(volatile uint8_t*)&SPI1->DR = data;
	
	while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
	
	return (*(volatile uint8_t*)&SPI1->DR);
}
