/*
 * hdd_clock.c
 *
 * Created: 2017-07-21 00:54:09
 * Author : Krisztian Stancz
 */ 

#include "hdd_clock.h"

void init_sensor(void)
{
	// Reset pin
	SENSOR_PORT &= ~(1 << PSENSOR);

	// Set as input
	SENSOR_DDR &= ~(1 << PSENSOR);
	
	// Set interrupt for rising and falling edge
	EICRA |= 1 << ISC00;

	// Enable interrupt
	EIMSK |= 1 << INT0;	

	return;
}

void init_color_leds(void)
{
	// Reset pins
	LED1_PORT &= ~(1 << PLED1);
	LED2_PORT &= ~(1 << PLED2);
		
	// Set as output
	LED1_DDR |= 1 << PLED1;
	LED2_DDR |= 1 << PLED2;
		
	return;
}

void init_button(void)
{
	// Reset pin
	BUTTON_PORT &= ~(1 << PBUTTON);

	// Set as input
	BUTTON_DDR &= ~(1 << PBUTTON);
	
	// Enable interrupt
	PCMSK0 |= 1 << BUTTON_INT;
	PCICR |=  1 << BUTTON_INT_GROUP;
	
	// Save initial button state
	button_state = get_button_state();
	
	return;
}

void init_info_led(void)
{
	// Reset pin
	INFOLED_PORT &= ~(1 << PINFOLED);

	// Set as output
	INFOLED_DDR |= 1 << PINFOLED;
	
	return;	
}

void init_clock_adjusts(void)
{
	// Reset pins
	HSET_PORT &= ~(1 << PHSET);
	MSET_PORT &= ~(1 << PMSET);
	
	// Set as input
	HSET_DDR &= ~(1 << PHSET);
	MSET_DDR &= ~(1 << PMSET);
	
	return;
}

void adjust_time(void)
{
	// Read hours and minutes value from potentiometers
	
	hours = (uint8_t) (ADC_read(ADC_CH_HOUR) / HOUR_SCALE);
	if (hours > 11)
		hours = 11;
		
	minutes = (uint8_t) (ADC_read(ADC_CH_MIN) / MIN_SCALE);
	if (minutes > 59)
		minutes = 59;
	
	return;
}

uint8_t get_button_state(void)
{
	return BUTTON_PIN & (1 << PBUTTON);
}

void wait_for_stable_spin(void)
{
	
	float hdd_period_current = 100;
	float hdd_period_previous;
	float hdd_period_error;
	
	// Turn on measurement peripherals
	freq_meas_init();

	// Calculate spinning period error
	do {
		_delay_ms(500);
		hdd_period_previous = hdd_period_current;
		hdd_period_current = get_period();
		hdd_period_error = ((hdd_period_current - hdd_period_previous) / hdd_period_previous) * 100;
	} while(hdd_period_error > ERROR_THRESHOLD);

	// Turn off measurement peripherals so it does not
	// disturb sensor interrupts
	freq_meas_deinit();
	
	// Save latest measurement data for calculations
	hdd_period = hdd_period_current;
	
	return;
}

void get_time_from_rtc(void)
{
	hours = read_rtc(HOUR);
	if (hours > 12)
		hours -= 12;
		
	minutes = read_rtc(MIN);
	
	return;
}