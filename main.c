#include "hdd_clock.h"
#include <avr/io.h>
#include <stdio.h>


#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

int main(void)
{

	/* 
	 * HDD Clock Setup
	 */

	// Initialize drivers
	UART_init();

	// Initialize peripherals
	init_color_leds();

	// Enable global interrupts
	sei();
	
	// Set up STDIO input and output buffer
	FILE UART_output = FDEV_SETUP_STREAM((void *)UART_send_character, NULL, _FDEV_SETUP_WRITE);
	stdout = &UART_output;
	FILE UART_input = FDEV_SETUP_STREAM(NULL, (void *)UART_get_character, _FDEV_SETUP_READ);
	stdin = &UART_input;

	// Try printf
	printf("Startup...\r\n");

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

	// Print out values for debugging
	printf("Time: %02d:%02d:%02d\n", hours, minutes, seconds);
	printf("HDD rotation freq: ");
	print_float((float) (1.0 / hdd_period), 2);
	printf(" Hz\n");
	printf("HDD rotation freq error: ");
	print_float((float) hdd_period_error, 2);
	printf(" %%\n");
	printf("Sensor is %d hour marks from 12\n", HOURS_TO_ZERO);
	printf("Sensor is %d ticks (DEGREES_TO_ZERO degrees) from next hour mark\n", sensor_counts);
	printf("Handle thickness is %d ticks (HANDLE_THICKNESS degrees)\n", thickness_counts);
	printf("One minute is %d ticks (6 degrees)\n");

	// Compensate handle positions for sensor placement
	adjust_time();

	/* 
	 * HDD Clock Start-up
	 */

	// Turn off LEDs
	init_color_leds();
	
	// Start the clock
	start_seconds_counter();
		
	// Show the time
	init_sensor();

	/*
	 * HDD Clock Normal Operation
	 */
	
	while (1) {
		
		// Showing the time is done from interrupts,
		// only handle other actions
		switch (state) {
		case UPDATE_TIME_FROM_RTC:
			get_time_from_rtc();
			adjust_time();
			state = SHOW_TIME;
			break;
		}
	}
	
	return 0;
}






