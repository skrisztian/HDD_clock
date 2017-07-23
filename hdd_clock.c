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
	
	// Reset hour counter used in interrupt
	hour_count = HOURS_TO_ZERO + hours - 1;

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
	
	double hdd_period_current = 100.0;
	double hdd_period_previous;
	double hdd_period_error;
	
	// Turn on measurement peripherals
	freq_meas_init();

	// Calculate spinning period error
	do {
		_delay_ms(500);
		hdd_period_previous = hdd_period_current;
		hdd_period_current = get_period() * ENCODER_MARKS;
		hdd_period_error = ((hdd_period_current - hdd_period_previous) / hdd_period_previous) * 100;
	} while (hdd_period_error > ERROR_THRESHOLD);

	// Turn off measurement peripherals so it does not
	// disturb sensor interrupts
	freq_meas_deinit();
	
	// Save latest measurement data for calculations
	hdd_period = hdd_period_current;
	
	return;
}

void get_time_from_rtc(void)
{
	// 12 o'clock = 0 hour
	hours = read_rtc(HOUR);
	if (hours >= 12)
		hours -= 12;
		
	minutes = read_rtc(MIN);
	seconds = read_rtc(SEC);
	
	return;
}

void calculate_timers(void)
{
	// Using timer clock prescaler 8, one clock period in sec
	double clock_period = 1.0 / (16000000 / 8);
	
	// Time required for 1 degree rotation
	double deg_period = hdd_period / 360;
	
	// Timer counts required for 1 degree rotation
	double deg_counts = deg_period / clock_period;
	
	// Timer counts required from the sensor until the next hour mark
	sensor_counts = (uint8_t) (DEGREES_TO_ZERO * deg_counts);
	
	// Timer counts required for handle thickness
	thickness_counts = (uint8_t) (HANDLE_THICKNESS * deg_counts);
	
	// Timer counts required for one minute (5 degrees)
	minute_counts = (uint8_t) (5 * deg_counts);

	return;
}


void init_timers(void)
{
	/*
	 * Use TC1 (16 bit) to fire interrupt every seconds
	 */
	
	// Set clock prescaler to 1024
	TCCR1B |= (1<<CS10) | (1<<CS12);
	
	// Set CTC mode
	TCCR1B |= 1 << WGM12;
	
	// Initialize counter
	TCNT1 = 0;
	
	// Set compare value for 1 sec at 16 MHz / 1024
	OCR1A = 15625;

	/*
	 * Use TC0 (8 bit) to set hour handle
	 */
	
	// Set clock prescaler to 8
	TCCR0B |= 1<<CS01;
	
	// Set CTC mode
	TCCR0B |= 1 << WGM12;
	
	// Initialize counter
	TCNT0 = 0;
	
	// Set compare value to correct sensor misalignment
	OCR0A = sensor_counts;
	
	// Set compare value for handle thickness
	OCR0B = thickness_counts;
	
	/*
	 * Use TC2 (8 bit) to set minute handle
	 */
	
	// Set clock prescaler to 8
	TCCR2B |= 1<<CS01;
	
	// Set CTC mode
	TCCR2B |= 1 << WGM12;
	
	// Initialize counter
	TCNT2 = 0;
	
	// Set compare value to minute position
	OCR2A = 0;
	
	// Set compare value for handle thickness
	OCR2B = thickness_counts;
	
	return;	
}

	// Enable compare interrupt
	// TIMSK0 |= 1 << OCIE0A;


ISR(INT0_vect)
{
	// This interrupt is fired when the encoder passes over the sensor

	// Hour_count also counts 5-minute units
	hour_count++;

	// Roll over. 12 o'clock = 0 hour but we correct for sensor position
	if (hour_count - HOURS_TO_ZERO >= 12)
		hour_count = HOURS_TO_ZERO;

	// We only show the hour handle in the correct hour
	// Correct sensor misalignment
	if (hour_count - HOURS_TO_ZERO == hours) {
		TCNT0 = 0;
		TIMSK0 |= 1 << OCIE0A;
	}
		
	//  Minute handle - TODO		
	
}

ISR(TIMER0_COMPA_vect)
{
	// This interrupt fires when it is time to turn on hour handle
	
	// Turn of interrupt
	TIMSK0 &= ~(1 << OCIE0A);
	
	// Turn on hour handle
	LED1_PORT |= 1 << PLED1;
	
	// Wait for handle thickness
	TCNT0 = 0;
	TIMSK0 |= 1 << OCIE0B;
}

ISR(TIMER0_COMPB_vect)
{
	// This interrupt fires when it is time to turn off hour handle
	
	// Turn off interrupt
	TIMSK0 &= ~(1 << OCIE0B);
	
	// Turn off hour handle
	LED1_PORT &= ~(1 << PLED1);
}




ISR(TIMER1_COMPA_vect)
{
	// Update seconds, minutes hours
	seconds++;

	// Roll over after 12h
	if (seconds >= 43200)
		seconds = 0;
		
	minutes = seconds / 60;
	hours = seconds / 3600;
}