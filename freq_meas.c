/*
 * freq_meas.c
 */ 

#include "freq_meas.h"

void freq_meas_init(void)
{
	// Using ICP1 - Timer/Counter1 Input Capture Timer

	// Reset IO pin and set as input
	ICR1_PORT &= ~(1 << PICR1);	ICR1_DDR &= ~(1 << PICR1);	
	// Clear then set prescaler
	TCCR1B &= ~(0x07);	TCCR1B |= CS_DIV_64;
	
	// Turn on noise canceler
	// TCCR1B |= 1 << ICNC1;
	
	// Capture on rising edge
	TCCR1B |= 1 << ICES1;
	
	// Enable input capture interrupt
	TIMSK1 |= 1 << ICIE1;		// Enable overflow interrupt	TIMSK1 |= 1 << TOIE1;		// Reset overflow counter
	overflow_cntr = 0;
}

void freq_meas_deinit(void)
{
	// Turn off input capture
	TCCR1B &= ~(0x07);	TCCR1B |= CS_NO_CLOCK;
	
	// Disable input capture interrupt
	TIMSK1 &= ~(1 << ICIE1);			// Disable overflow interrupt	TIMSK1 &= ~(1 << TOIE1);	
	return;
}

float get_period(void)
{	
	// Atomic copy
	cli();
	uint16_t first_signal = signal1;
	uint16_t second_signal = signal2;
	uint16_t number_of_overflows = overflows;
	sei();
	
	int32_t diff = (int32_t) second_signal - (int32_t) first_signal;
	uint32_t step_no = (((uint32_t) number_of_overflows) * 65536 ) + diff;
	
	float signal_period = ((float) step_no) * (4.0 / 1000000);

	return signal_period;
}

float get_frequency(void)
{
	float signal_period = get_period();	
	float frequency = 1.0 / signal_period;
	
	return frequency;
}

float get_rpm(void)
{
	float frequency = get_frequency();
	float rpm = frequency / 60.0;

	return rpm;
}

// Input capture interrupt
ISR(TIMER1_CAPT_vect)
{
	overflows = overflow_cntr;
	overflow_cntr = 0;
	signal1 = signal2;
	signal2 = ICR1;
}

// TC1 timer overflow interrupt
ISR(TIMER1_OVF_vect)
{
	//Count the number of overflows
	overflow_cntr++;
}