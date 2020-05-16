/* 
 *  ESP8266-based RTC
 *  part of the PCB WordClock project: http://embedblog.eu/?p=636
 *  martin@embedblog.eu
 *  this is a simple driver for the MAX7219 driver IC
 */

#include <SPI.h>

#define REG_NOOP    0x0
#define REG_DIG0    0x1
#define REG_DIG1    0x2
#define REG_DIG2    0x3
#define REG_DIG3    0x4
#define REG_DIG4    0x5
#define REG_DIG5    0x6
#define REG_DIG6    0x7
#define REG_DIG7    0x8
#define REG_DECODE  0x9
#define REG_INTEN   0xA
#define REG_SCAN    0xB
#define REG_SDOWN   0xC
#define REG_TEST    0xF

#define DEVICES     3

#define CS_HIGH     digitalWrite(12, HIGH);
#define CS_LOW      digitalWrite(12, LOW);


void max7219_init(void)
{
  //configure SPI
  SPI.begin();
  //SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

  //configure GPIO for SPI
  
  //configure CS pin
  pinMode(12, OUTPUT);
  CS_HIGH;
  
  delay(10);
  
  //set default values
  max7219_writeAllRegisters(REG_SDOWN, 0x01);       //power up
  delayMicroseconds(1);
  max7219_setIntensity(0x0F);                       //lowest intensity
  delayMicroseconds(1);
  max7219_zeroDisplay();                            //zero anything left in the memory
  delayMicroseconds(1);
  max7219_writeAllRegisters(REG_SCAN, 0x07);        //enable all digits
  delayMicroseconds(1);
  max7219_writeAllRegisters(REG_TEST, 0x01);        //LED test
  //delay(250);  
  //max7219_writeAllRegisters(REG_TEST, 0x00);
}

void max7219_writeSingleRegister(uint8_t reg, uint8_t data)
{
  /*uint16_t out = ((reg & 0xF) << 8) | data;
  SPI1->DR = out;
  while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);
  delayMicroseconds(1);*/
  SPI.transfer(reg);
  SPI.transfer(data);
}

void max7219_writeAllRegisters(uint8_t reg, uint8_t data)
{
  CS_LOW;
  for (uint8_t i = 0; i < DEVICES; i++) max7219_writeSingleRegister(reg, data);
  CS_HIGH;
}

void max7219_setIntensity(uint8_t intensity)      //from 0x00 to 0x0F (ie 15)
{
  if (intensity > 0x0F) intensity = 0x0F;
  
  max7219_writeAllRegisters(REG_INTEN, intensity);
}

void max7219_zeroDisplay(void)
{
  for (uint8_t i = 0; i < 8; i++)         //zero display
  {
    max7219_writeAllRegisters(REG_DIG0 + i, 0x00);
    delayMicroseconds(10);
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
