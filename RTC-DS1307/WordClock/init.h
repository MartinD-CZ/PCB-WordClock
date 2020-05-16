#ifndef _INIT_H
#define _INIT_H

#define LED_ON		GPIOA->BSRR = GPIO_BSRR_BS_4
#define LED_OFF		GPIOA->BSRR = GPIO_BSRR_BR_4

void init_RCC(void);
void init_GPIO(void);
void init_TIM16(void);

#endif
