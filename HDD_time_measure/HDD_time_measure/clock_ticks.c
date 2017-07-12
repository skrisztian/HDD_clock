#include <avr/io.h>
#include <avr/interrupt.h>
#include "time_meas.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

uint8_t x = 20;
uint8_t hour_led_on = 0;
uint8_t minute_led_on = 0;


void init_pin_change(void)
{
	// Enable interrupt on pin
	EIMSK |= 1 << INT0;
		
	// rising edge
	EICRA |= (1 << ISC01) | (1 << ISC00);
	
	return;
}

void init_timer0(void)
{
	  //timer0 prescaler 256 -> we need 69.44 steps
	  OCR0A = 7;
	  TCCR0A |= 1<< WGM01;
	  TIMSK0 |= 1 << OCIE0A;		// enable CTC interrupt
	  
	  return;
}

void init_timer2(void)
{
	//timer0 prescaler 1024
	OCR2A = x;
	TCCR2A |= 1<< WGM21;		// Timer/Counter Mode CTC
	TIMSK2 |= 1 << OCIE2A;		// enable CTC interrupt
	TCCR2B |= (1<< CS22);		// turn on timer
	TCCR2B |= (1<< CS20);		// prescaler 1024
	
	return;
}

void ticks_init(void)
{
	init_pin_change();
	init_timer0();
	init_timer2();
}

ISR(INT0_vect)
{
	OCR0A = x + (hours * 10.9);
	PORTD &= ~(1 << PORTD6);
		// turn D6 LED pin on
	TCCR0B |= (1<< CS02);		// turn on timer
	TCCR0B |= (1<< CS00);
	OCR2A = x + (minutes * 2.18);
	//PORTD |= (1 << PORTD6);		// turn D6 LED pin on
	TCCR2B |= (1<< CS22);		// turn on timer
	TCCR2B |= (1<< CS20);		// turn on timer with 1024 prescaler
	// hour_led_on = 0;
	// minute_led_on = 0;
}

ISR(TIMER0_COMPA_vect)
{
	PORTD |= (1 << PORTD6);				// turn led pin off
	TCCR0B &= ~(1 << CS02);			// turn off timer
	TCNT0 = 0;
}

ISR(TIMER2_COMPA_vect)
{
	if (minute_led_on == 1) {
		//PORTD &= ~(1 << PORTD7);
		PORTD |= 1 << PORTD7;		// turn led pin off
		TCCR2B &= ~(1 << CS22);			// turn off timer
		TCCR2B &= ~(1 << CS20);			// turn off timer
		TCNT2 = 0;
		minute_led_on = 0;
	} else {
		PORTD &= ~(1 << PORTD7);
		//PORTD |= 1 << PORTD7;
		TCCR2B &= ~(1 << CS22);			// turn off timer
		TCCR2B &= ~(1 << CS20);			// turn off timer
		OCR2A = 20;
		TCNT0 = 0;
		TCCR2B |=1 << CS22;			// turn off timer
		TCCR2B |=1 << CS20;			// turn off timer
		minute_led_on = 1;
	}
}
