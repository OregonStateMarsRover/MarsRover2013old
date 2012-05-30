#include "Timer.h"

void InitTimers(void)
{
	TCC0.PER = 0xFFFF; //top
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc; // 16 MHz clock div by 1024: 64us per tick
}

uint16_t CurrentTime(void)
{
	return TCC0.CNT;
}

uint16_t GetSpanUs(Timer * t)
{
	uint16_t time = CurrentTime();
	uint16_t span = time - t->startTime;
	return span;
}

void StartTimer(Timer * t)
{
	t->startTime = CurrentTime();
}


