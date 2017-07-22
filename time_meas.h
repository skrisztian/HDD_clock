#ifndef _TIME_MEAS_H
#define _TIME_MEAS_H

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

// define global vars
#define steps_to_one_sec 15625
uint16_t compare_cntr;
volatile uint8_t minutes;
volatile uint8_t hours;

void time_meas_init();

#endif // _FREQ_MEAS_H
