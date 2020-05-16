#ifndef _DS1307_H
#define _DS1307_H

void ds1307_setTime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t month, uint8_t year, uint8_t day_of_week);
void ds1307_loadTime(void);
void ds1307_readTime(uint8_t* seconds, uint8_t* minutes, uint8_t* hours, uint8_t* day, uint8_t* month, uint8_t* year, uint8_t* day_of_week);

#endif
