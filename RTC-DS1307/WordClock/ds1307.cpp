#include "stdint.h"
#include "ds1307.h"
#include "i2c.h"

uint8_t registers[8];

void ds1307_setTime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t month, uint8_t year, uint8_t day_of_week)
{
	//set time
	i2c_writeBuffer(0x00);																							//register pointer address
	i2c_writeBuffer(((seconds / 10) << 4) | ((seconds % 10) << 0));			//seconds, enable oscilator
	i2c_writeBuffer(((minutes / 10) << 4) | ((minutes % 10) << 0));			//minutes
	//i2c_writeBuffer(((hours / 10) << 4) | ((hours % 10) << 0));				//hours - 24 hours format
	i2c_writeBuffer((1 << 6) | ((hours / 10) << 4) | ((hours % 10) << 0));
	i2c_writeBuffer(day_of_week << 0);																	//day of week
	i2c_writeBuffer(((day / 10) << 4) | ((day % 10) << 0));							//day
	i2c_writeBuffer(((month / 10) << 4) | ((month % 10) << 0));					//month
	i2c_writeBuffer(((year / 10) << 4) | ((year % 10) << 0));						//year
	i2c_transmit();
}

void ds1307_loadTime(void)
{
	i2c_writeBuffer(0x00);
	i2c_transmit();
		
	i2c_request(7);
	for (uint8_t i = 0; i < 7; i++)
		registers[i] = i2c_readBuffer(i);
}

void ds1307_readTime(uint8_t* seconds, uint8_t* minutes, uint8_t* hours, uint8_t* day, uint8_t* month, uint8_t* year, uint8_t* day_of_week)
{
	*seconds = (((registers[0] >> 4) & 0x07) * 10) + (registers[0] & 0x0F);
	*minutes = (((registers[1] >> 4) & 0x07) * 10) + (registers[1] & 0x0F);
	*hours = (((registers[2] >> 4) & 0x01) * 10) + (registers[2] & 0x0F);
	*day = (((registers[4] >> 4) & 0x03) * 10) + (registers[4] & 0x0F);
	*month = (((registers[5] >> 4) & 0x01) * 10) + (registers[5] & 0x0F);
	*year = (((registers[6] >> 4) & 0x0F) * 10) + (registers[6] & 0x0F);
	*day_of_week = registers[3] & 0x03;
}
