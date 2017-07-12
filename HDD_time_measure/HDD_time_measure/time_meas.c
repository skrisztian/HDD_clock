#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "time_meas.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/delay.h>

#define ICR1_DDR			DDRB
#define ICR1_DDR_POS		DDRB0


void time_meas_init()
{
	/**************
	 * TC1 CONFIG *
	 **************/
	// TODO:
	// Configure the TC1 timer properly :)
	// Set prescaler to clkI/O/1024
	//timer0 prescaler 1024
	OCR1A = steps_to_one_sec;
	TCCR1A |= 1<< WGM12;		// Timer/Counter Mode CTC
	TIMSK1 |= 1 << OCIE1A;		// enable CTC interrupt
	TCCR1B |= (1<< CS10);		// turn on timer
	TCCR1B |= (1<< CS12);		// prescaler 1024	
	//
	compare_cntr = 0;
	minutes = 0;
	hours = 0;
	
	// init led
	DDRB |= 1 << DDRB5;
	
}

//Count the number of overflows

ISR(TIMER1_COMPA_vect)
{
	if (compare_cntr <= 59) {
		TCNT1 = 0;
		compare_cntr++;
		PINB |= 1 << PINB5;
	}
	else {
		TCNT1 = 0;
		minutes++;
		compare_cntr = 0;
	}
	
	if (minutes == 60) {
		minutes = 0;
		hours++;
	} 
	
	if (hours == 12) 
		hours = 0;
}