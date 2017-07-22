#ifndef _CLOCK_TICKS_H
#define _CLOCK_TICKS_H

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

void init_pin_change(void);
void init_timer0(void);
void init_timer2(void);
void ticks_init(void);

#endif // _FREQ_MEAS_H

