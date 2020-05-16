/* ====================================================
 * STM32 OPEN SOURCE MULTIMETER
 * embedblog.eu/dmm
 * license: CC BY-NC-SA 4.0
 * ==================================================== */

#include "stm32f0xx.h"
#include "uart.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define F_CPU 48000000UL
#ifndef F_CPU
#error "F_CPU not defined for uart.cpp!"
#endif

#define UART_TXBUF		128
#define UART_RXBUF		32

char bufferTX[UART_TXBUF];
volatile char bufferRX[UART_RXBUF];
volatile uint8_t posRX = 0;

void uart_init(uint32_t baudrate)
{
	//enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	//setup GPIO: PA2 as TX, PA3 as RX
	GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3;
	GPIOA->AFR[0] |= (1 << 8) | (1 << 12);
	
	//calculate dividers for baud rate generator
	float divider = (float)F_CPU / (16 * baudrate);
	uint16_t divider_integral = floorf(divider);
	uint16_t divider_fractional = (uint16_t)((divider - divider_integral) / (1.0f/16) + 0.5f);
	USART1->BRR = (divider_integral << 4) | (divider_fractional & 0xF);
	
	//setup DMA1CH2 for TX
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel2->CPAR = (uint32_t)(&(USART1->TDR));
	DMA1_Channel2->CMAR = (uint32_t)bufferTX;
	USART1->CR3 = USART_CR3_DMAT;
	
	//enable RX interrupts
	NVIC_SetPriority(USART1_IRQn, 4);
	NVIC_EnableIRQ(USART1_IRQn);
	
	//enable the USART, transmitter and receiver + interrupt
	USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}

void uart_print(char *msg)
{
	//wait for any ongoing transmission complete flag and disable DMA
	while ((USART1->ISR & USART_ISR_TC) == 0x00);
	DMA1_Channel2->CCR = 0x00;
	
	//copy the message to TX buffer
	int16_t pos = -1;
	while (msg[++pos] != '\0')
		bufferTX[pos] = msg[pos];
	
	//enable DMA
	DMA1_Channel2->CNDTR = pos;
	DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_EN | DMA_CCR_DIR;
}

void uart_printf(char *szFormat, ...)
{
	//wait for any ongoing transmission complete flag and disable DMA
	while ((USART1->ISR & USART_ISR_TC) == 0x00);
	DMA1_Channel2->CCR = 0x00;
	
	//format the string
	va_list pArgs;
	va_start(pArgs, szFormat);
	uint16_t numchars = vsnprintf(bufferTX, UART_TXBUF - 1, szFormat, pArgs);
	va_end(pArgs);
	
	//enable DMA
	DMA1_Channel2->CNDTR = numchars;
	DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_EN | DMA_CCR_DIR;
}

extern "C"
{
	void USART1_IRQHandler()
	{
		if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
			bufferRX[posRX++] = USART1->RDR;
	}
}
