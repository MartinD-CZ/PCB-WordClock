#ifndef _WCLOCK_H
#define _WCLOCK_H

void wordclock_init(void);
void wordclock_update(uint8_t hours, uint8_t minutes);
void wordclock_sendBuffer(void);

#endif
