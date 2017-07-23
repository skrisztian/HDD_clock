#ifndef _FREQ_MEAS_H
#define _FREQ_MEAS_H

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/delay.h>

#define PICR1				0
#define ICR1_DDR			DDRB
#define ICR1_PORT			PORTB
#define ICR1_PIN			PINB

#define CS_NO_CLOCK			0
#define CS_DIV_1			1
#define CS_DIV_8			2
#define CS_DIV_64			3
#define CS_DIV_256			4
#define CS_DIV_1024			5
#define CS_EXT_CLOCK_FALL	6
#define CS_EXT_CLOCK_RISE	7

volatile uint16_t overflow_cntr;
volatile uint16_t overflows;
volatile uint16_t signal1;
volatile uint16_t signal2;

void freq_meas_init(void);
void freq_meas_deinit(void);
double get_period(void);
double get_freq(void);
double get_rpm(void);

#endif // _FREQ_MEAS_H
