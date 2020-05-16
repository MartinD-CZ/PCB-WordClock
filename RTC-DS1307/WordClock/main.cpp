#include "stm32f0xx.h"
#include "init.h"
#include "delay.h"
#include "i2c.h"
#include "uart.h"
#include "ds1307.h"
#include "wordclock.h"

uint8_t rtcSec, rtcMin, rtcHrs, rtcDay, rtcMon, rtcYear, rtcDOW;
extern volatile bool update, btn_hrs, btn_min;

//SETTINGS
//#define UART_LOAD
#define UART_DEBUG

int main(void)
{
	init_RCC();
	init_GPIO();
	init_TIM16();
	
	delay_init();
	delay_ms(1000);
	
	uart_init(115200);
	uart_print((char*)"\n\nDevice started\n");
	
	i2c_init();
	i2c_setSlaveAddress(0x68);
	
	wordclock_init();
	
	#ifdef UART_LOAD
	volatile uint8_t temp = USART1->RDR;
	uart_print((char*)"Send any char via serial to set clock\n");
	while (USART1->RDR == 0x00);
	ds1307_setTime(0, 22, 10, 28, 3, 20, 5);
	uart_print((char*)"Time set!\n");
	#endif
	
	while (1)
	{
		if (update)
		{
			ds1307_loadTime();
			ds1307_readTime(&rtcSec, &rtcMin, &rtcHrs, &rtcDay, &rtcMon, &rtcYear, &rtcDOW);
			
			if (btn_min)
			{
				btn_min = false;
				rtcMin++;
				if (rtcMin > 59) rtcHrs = 0;
				ds1307_setTime(rtcSec, rtcMin, rtcHrs, rtcDay, rtcMon, rtcYear, rtcDOW);
			}
			
			if (btn_hrs)
			{
				btn_hrs = false;
				rtcHrs++;
				if (rtcHrs > 12) rtcHrs = 1;
				ds1307_setTime(rtcSec, rtcMin, rtcHrs, rtcDay, rtcMon, rtcYear, rtcDOW);
			}
			
			wordclock_update(rtcHrs, rtcMin);
		
			#ifdef UART_DEBUG
			switch (rtcDOW)
			{
				case 0: uart_print((char*)"Mon"); break;
				case 1: uart_print((char*)"Tue"); break;
				case 2: uart_print((char*)"Wed"); break;
				case 3: uart_print((char*)"Thu"); break;
				case 4: uart_print((char*)"Fri"); break;
				case 5: uart_print((char*)"Sat"); break;
				case 6: uart_print((char*)"Sun"); break;
				default: uart_print((char*)"ERR"); break;
			}
		
			uart_printf((char*)" %02d.%02d.20%02d   %02d:%02d:%02d\n", rtcDay, rtcMon, rtcYear, rtcHrs, rtcMin, rtcSec);	
			#endif
			
			update = false;
		}
		
		
	}
}
