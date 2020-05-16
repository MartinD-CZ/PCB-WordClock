#include "init.h"
#include "stm32f0xx.h"

volatile bool update = false, btn_hrs = false, btn_min = false;

void init_RCC(void)
{
	//set flash latency
	FLASH->ACR |= FLASH_ACR_LATENCY;
  
  //select PLL source (8 MHz / 2)
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2;
	
	//select PLL multiplier x12
	RCC->CFGR |= RCC_CFGR_PLLMUL12;
	
	//enable PLL
	RCC->CR |= RCC_CR_PLLON;
	
	//select PLL as system clock
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	//enable GPIO clocks
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOFEN;
	
	//enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
}

void init_GPIO(void)
{
	//PA4 as output for LED
	GPIOA->MODER |= GPIO_MODER_MODER4_0;
	
	//PA0:1 as interrupt inputs
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA | SYSCFG_EXTICR1_EXTI1_PA;
	EXTI->IMR |= EXTI_IMR_IM0 | EXTI_IMR_IM1;
	EXTI->FTSR |= EXTI_FTSR_FT0 | EXTI_FTSR_FT1;
	
	NVIC_SetPriority(EXTI0_1_IRQn, 7);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void init_TIM16(void)											//used for periodic 1s interrupt generation
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;		//enable clock to Timer 16
	TIM16->PSC = 47999;											//set prescaler (48k - 1)
	TIM16->ARR = 1000;											//in ms
	TIM16->EGR |= TIM_EGR_UG;								//update registers
	TIM16->DIER |= TIM_DIER_UIE;						//enable interrupt on timer overflow (aka update event)

	NVIC_SetPriority(TIM16_IRQn, 1);
	NVIC_EnableIRQ(TIM16_IRQn);
	
	TIM16->CR1 |= TIM_CR1_CEN;							//enable timer
}

extern "C"
{
	void TIM16_IRQHandler(void)
	{
		update = true;
		
		TIM16->SR &=~TIM_SR_UIF;
	}
	
	void EXTI0_1_IRQHandler(void)
	{
		if ((EXTI->PR & EXTI_PR_PR0) != 0)
		{
			btn_hrs = true;
			update = true;
			EXTI->PR |= EXTI_PR_PR0;
		}
		
		if ((EXTI->PR & EXTI_PR_PR1) != 0)
		{
			btn_hrs = true;
			update = true;
			EXTI->PR |= EXTI_PR_PR1;
		}
	}
}
