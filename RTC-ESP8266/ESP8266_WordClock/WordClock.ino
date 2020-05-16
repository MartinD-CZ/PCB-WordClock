/* 
 *  ESP8266-based RTC
 *  part of the PCB WordClock project: http://embedblog.eu/?p=636
 *  martin@embedblog.eu
 *  this file does all the logic for the WordClock
 */

uint8_t digitBuffers[24];

#define SBI(buf, bit)    digitBuffers[buf] |= (1 << bit)
#define CS_HIGH     digitalWrite(12, HIGH);
#define CS_LOW      digitalWrite(12, LOW);

void wordclock_update(uint8_t hours, uint8_t minutes)
{
  for (uint8_t i = 0; i < 24; i++) digitBuffers[i] = 0x00;
  
  //always on
  SBI(0, 6);      //I
  SBI(1, 6);      //T
  SBI(3, 6);      //I
  SBI(4, 6);      //S
  SBI(22, 7);     //O
  SBI(21, 7);     //'
  SBI(20, 7);     //C
  SBI(8, 7);      //L
  SBI(9, 7);      //O
  SBI(10, 7);     //C
  SBI(11, 7);     //K
  
  if ((minutes <= 30) && (minutes > 0)) {SBI(8, 3); SBI(9, 3); SBI(10, 3); SBI(11, 3);}   //PAST
  else if (minutes != 0) {SBI(0, 7); SBI(1, 7); hours++;}                                 //TO + increment hours
  
  //minutes
  if (minutes == 15 || minutes == 45)     
  {
    SBI(0, 5); SBI(1, 5); SBI(2, 5); SBI(3, 5); SBI(4, 5); SBI(5, 5); SBI(6, 5);    //QUARTER
  }
  else if (minutes == 30)                 
  {
    SBI(12, 6); SBI(13, 6); SBI(14, 6); SBI(15, 6);                                 //HALF
  }
  else if (minutes == 10 || minutes == 50)
  {
    SBI(8, 4); SBI(10, 4); SBI(11, 4);                                              //TEN
  }
  else if (minutes == 11 || minutes == 49)
  {
    SBI(0, 1); SBI(1, 1); SBI(2, 1); SBI(3, 1); SBI(4, 1); SBI(5, 1);               //ELEVEN
  }
  else if (minutes == 12 || minutes == 48)
  {
    SBI(6, 1); SBI(7, 1); SBI(12, 4); SBI(13, 4); SBI(14, 4); SBI(15, 4);           //TWELVE
  }
  else if (minutes == 13 || minutes == 47)
  {
    SBI(0, 0); SBI(1, 0); SBI(2, 0); SBI(3, 0);                                     //THIR
    SBI(4, 0); SBI(5, 0); SBI(6, 0); SBI(7, 0);                                     //TEEN
  }
  else
  {
    if (minutes > 30) minutes = 60 - minutes;
    
    switch (minutes % 10)
    {
      case 1: SBI(0, 4); SBI(1, 4); SBI(2, 4); break;
      case 2: SBI(3, 4); SBI(4, 4); SBI(5, 4); break;
      case 3: SBI(7, 4); SBI(8, 5); SBI(9, 5); SBI(10, 5); SBI(11, 5); break;
      case 4: SBI(0, 3); SBI(1, 3); SBI(5, 3); SBI(6, 3); break;
      case 5: SBI(0, 3); SBI(2, 3); SBI(3, 3); SBI(4, 3); break;
      case 6: SBI(13, 5); SBI(14, 5); SBI(15, 5); break;
      case 7: SBI(0, 2); SBI(1, 2); SBI(2, 2); SBI(3, 2); SBI(4, 2); break;
      case 8: SBI(3, 2); SBI(5, 2); SBI(6, 2); SBI(7, 2); SBI(8, 4); break;
      case 9: SBI(4, 2); SBI(5, 2); SBI(9, 4); SBI(10, 4); break;
    }
    
    if ((minutes > 13) && (minutes < 20)) {SBI(4, 0); SBI(5, 0); SBI(6, 0); SBI(7, 0);}       //add the "teen"
    
    //add the "twenty"
    if ((minutes >= 20) && (minutes <= 40)) {SBI(6, 6); SBI(7, 6); SBI(8, 6); SBI(9, 6);  SBI(10, 6); SBI(11, 6);}
  }
  
  //hours
  if (hours == 0) hours = 12;
  if (hours > 12) hours = hours - 12;
  
  switch (hours)
  {
    case 1: SBI(2, 7); SBI(3, 7); SBI(4, 7); break;
    case 2: SBI(5, 7); SBI(6, 7); SBI(7, 7); break;
    case 3: SBI(5, 7); SBI(12, 3); SBI(13, 3); SBI(14, 3); SBI(15, 3); break;
    case 4: SBI(12, 7); SBI(13, 7); SBI(13, 1); SBI(14, 1); break;
    case 5: SBI(12, 7); SBI(14, 7); SBI(15, 7); SBI(12, 1); break;
    case 6: SBI(9, 2); SBI(10, 2); SBI(11, 2); break;
    case 7: SBI(19, 0); SBI(18, 0); SBI(17, 0); SBI(16, 0); SBI(8, 0); break;
    case 8: SBI(16, 0); SBI(9, 0); SBI(10, 0); SBI(11, 0); SBI(12, 2); break;
    case 9: SBI(8, 0); SBI(9, 0); SBI(13, 2); SBI(14, 2); break;
    case 10: SBI(12, 2); SBI(14, 2); SBI(15, 2); break;
    case 11: SBI(19, 7); SBI(18, 7); SBI(17, 7); SBI(16, 7); SBI(12, 0); SBI(13, 0); break;
    case 12: SBI(14, 0); SBI(15, 0); SBI(8, 1); SBI(9, 1); SBI(10, 1); SBI(11, 1); break;
    default: SBI(19, 7); SBI(17, 7); SBI(12, 0);
  }
  
  wordclock_sendBuffer();
}

void wordclock_sendBuffer(void)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    CS_LOW;
    max7219_writeSingleRegister(0x01 + i, digitBuffers[i + 16]);
    max7219_writeSingleRegister(0x01 + i, digitBuffers[i + 8]);
    max7219_writeSingleRegister(0x01 + i, digitBuffers[i + 0]);
    CS_HIGH;
    delayMicroseconds(1);
  }
}
