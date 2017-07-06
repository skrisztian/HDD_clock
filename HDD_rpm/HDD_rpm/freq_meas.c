#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/delay.h>

#define ICR1_DDR			DDRB
#define ICR1_DDR_POS		DDRB0

// define global vars
uint16_t overflow_cntr;
uint16_t overflows;
uint16_t signal1;
uint16_t signal2;


void freq_meas_init()
{
	/**************
	 * TC1 CONFIG *
	 **************/
	// TODO:
	// Configure the TC1 timer properly :)
	// Set prescaler to clkI/O/64
	TCCR1B &= ~(1 << CS12);	TCCR1B |= 1 << CS11;
	TCCR1B |= 1 << CS10;
	
	//Turn noise canceler
	TCCR1B |= 1 << ICNC1;
	
	//Capture rising edge
	TCCR1B |= 1 << ICES1;
	
	//enable input capture interrupt
	TIMSK1 |= 1 << ICIE1;		//enable overflow interrupt	TIMSK1 |= 1 << TOIE1;		//Configure PB0 as intput 	ICR1_DDR &= ~(1 << DDRB0);	
	//
	overflow_cntr = 0;
}

// TODO:
// Write this function. It returns the measured frequency in Hz

float get_freq()
{	
	cli();
	uint16_t first_signal = signal1;
	uint16_t second_signal = signal2;
	uint16_t number_of_overflows = overflows;
	sei();
	
	int32_t diff = (int32_t)second_signal - (int32_t)first_signal;
	uint32_t step_no = (((uint32_t)number_of_overflows) * 65536 ) + diff;
	
	float signal_period = ((float) step_no) * (4.0 / 1000000);
	float frequency = 1/signal_period;
	
	return frequency;
}

ISR(TIMER1_CAPT_vect)
{
	overflows = overflow_cntr;
	overflow_cntr = 0;
	signal1 = signal2;
	signal2 = ICR1;
}

//Count the number of overflows

ISR(TIMER1_OVF_vect)
{
	overflow_cntr++;
}