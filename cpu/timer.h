#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern uint32_t tick;

void init_timer(uint32_t freq);
void sleep(uint8_t seconds);

#endif
