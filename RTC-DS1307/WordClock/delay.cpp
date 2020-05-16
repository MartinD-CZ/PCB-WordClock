#include "stm32f0xx.h"
#include "delay.h"

void delay_ms(uint16_t ms)
{
	//set prescaler (4799 + 1)
	TIM17->PSC = (F_CPU/1000 - 1);
	
	//update registers
	TIM17->EGR |= TIM_EGR_UG;
	
	//zero the counter
	TIM17->CNT = 0x00;
	
	//enable timer
	TIM17->CR1 = TIM_CR1_CEN;
	
	//wait for compare
	while (ms > TIM17->CNT);
		
	//disable timer
	TIM17->CR1 = 0x00;
}

void delay_us(uint16_t us)
{
	//set prescaler (47 + 1)
	TIM17->PSC = (F_CPU/1000000 - 1);
	
	//update registers
	TIM17->EGR |= TIM_EGR_UG;
	
	//zero the counter
	TIM17->CNT = 0x00;
	
	//enable timer
	TIM17->CR1 = TIM_CR1_CEN;
	
	//wait for compare
	while (us > TIM17->CNT);
		
	//disable timer
	TIM17->CR1 = 0x00;
}

void delay_init()
{
	//enable clock to Timer 4
	RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
}
