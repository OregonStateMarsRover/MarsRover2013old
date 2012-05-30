#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define FROM_uS(x) (x / 64)

struct STimer
{
	volatile uint16_t startTime;
};

typedef struct STimer Timer;
void InitTimers(void);
uint16_t CurrentTime(void);
uint16_t GetSpanUs(Timer * t);
void StartTimer(Timer * t);

#endif

