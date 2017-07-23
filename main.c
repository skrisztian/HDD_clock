#include "hdd_clock.h"
#include <avr/io.h>
#include <stdio.h>


#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

int main(void)
{

	// Initialize drivers
	UART_init();
	ADC_init();

	// Initialize peripherals
	init_color_leds();
	init_button();
	init_info_led();
	init_clock_adjusts();

	// Enable global interrupts
	sei();
	
	// Setting up STDIO input and output buffer
	FILE UART_output = FDEV_SETUP_STREAM((void *)UART_send_character, NULL, _FDEV_SETUP_WRITE);
	stdout = &UART_output;
	FILE UART_input = FDEV_SETUP_STREAM(NULL, (void *)UART_get_character, _FDEV_SETUP_READ);
	stdin = &UART_input;

	// Try printf
	printf("Startup...\r\n");

	/* START HERE */

	// Turn on one color of LEDs
	LED1_PORT |= 1 << PLED1;
	_delay_ms(1000);

	// Turn on the other color of LEDs
	LED1_PORT &= ~(1 << PLED1);
	LED2_PORT |= 1 << PLED2;
	_delay_ms(1000);
	
	// Turn on both colors of LEDs
	LED1_PORT |= 1 << PLED1;
	LED2_PORT |= 1 << PLED2;
	
	// Wait until the disk rotation speed
	// stabilizes. Measure turning speed.
	wait_for_stable_spin();
		
	// Calculate data for timers, then set parameters.
	calculate_timers();
	init_timers();
		
	// Read actual time from RTC
	get_time_from_rtc();
	
	// Start HHD Clock 
	init_sensor();

	// Use timers



}






