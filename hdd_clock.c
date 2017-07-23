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
	
	// Reset hour counter used in interrupt
	encoder_position = 255;
	
	// Enable INT0 interrupt
	EIMSK |= 1 << INT0;

	return;
	
	// TODO check if encoder_position 255 is ok for init value
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
	default_button_state = get_button_state();
	
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

void init_clock_adc(void)
{
	// Reset pins
	HSET_PORT &= ~(1 << PHSET);
	MSET_PORT &= ~(1 << PMSET);
	
	// Set as input
	HSET_DDR &= ~(1 << PHSET);
	MSET_DDR &= ~(1 << PMSET);
	
	return;
}

void get_time_from_adc(void)
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
	return (BUTTON_PIN & (1 << PBUTTON)) ? 1 : 0;
}

void wait_for_stable_spin(void)
{
	
	double hdd_period_current = 100.0;
	double hdd_period_previous;
	
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

void print_float(float number, uint8_t decimals)
{
	// Prints floating point number to stdout
	// up to the given decimals

	// Print the integer portion
	printf("%d", (int) number);

	// Print the decimal point
	if (decimals > 0) {
		printf(".");
	}

	// Print decimals after the dot up to the allowed number of decimals
	uint8_t counter = 0;
	while (number - (int) number > 0) {
		counter++;
		if (counter > decimals) {
			break;
			} else {
			number = (number - (int) number) * 10;
			printf ("%d", (int) number);
		}
	}

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

void adjust_time(void)
{
	// Adjust the position of the handles to
	// roughly compensate for sensor position

	hours += HOURS_TO_ZERO;
	if (hours >= 12)
		hours -= 12;
	
	minutes += (HOURS_TO_ZERO * 5);
	if (minutes >= 60)
		minutes -= 60;

	five_minutes = minutes / 5;
		
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

void start_seconds_counter(void)
{
	TIMSK1 |= 1 << OCIE1A;
	return;
}

void stop_seconds_counter(void)
{
	TIMSK1 &= ~(1 << OCIE1A);
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


ISR(INT0_vect)
{
	// This interrupt fires when the encoder passes over the sensor

	encoder_position++;

	// Roll over. 12 o'clock = 0 hour
	if (encoder_position >= 12)
		encoder_position = 0;

	// Only show hour handle in the correct hour
	// Start timer to correct sensor misalignment
	if (encoder_position == hours) {
		TCNT0 = 0;
		TIMSK0 |= 1 << OCIE0A;
	}
	
	// Only show minute handle in the correct minute
	// Set and start timer to correct sensor misalignment and show minute
	if (encoder_position == five_minutes) {
		TCNT0 = 0;
		OCR2A = minute_timer_counts;
		TIMSK2 |= 1 << OCIE0A;
	}
	
	// TODO: check if minute_counter_counts values fit into 255
}

ISR(TIMER0_COMPA_vect)
{
	// This interrupt fires when it is time to turn on the hour handle
	
	// Turn off interrupt
	TIMSK0 &= ~(1 << OCIE0A);
	
	// Turn on hour handle
	LED1_PORT |= 1 << PLED1;
	
	// Wait for handle thickness
	TCNT0 = 0;
	TIMSK0 |= 1 << OCIE0B;
}

ISR(TIMER0_COMPB_vect)
{
	// This interrupt fires when it is time to turn off the hour handle
	
	// Turn off interrupt
	TIMSK0 &= ~(1 << OCIE0B);
	
	// Turn off hour handle
	LED1_PORT &= ~(1 << PLED1);
}

ISR(TIMER2_COMPA_vect)
{
	// This interrupt fires when it is time to turn on the minute handle
	
	// Turn off interrupt
	TIMSK2 &= ~(1 << OCIE2A);
	
	// Turn on minute handle
	LED2_PORT |= 1 << PLED2;
	
	// Wait for handle thickness
	TCNT2 = 0;
	TIMSK2 |= 1 << OCIE2B;
}

ISR(TIMER2_COMPB_vect)
{
	// This interrupt fires when it is time to turn off the minute handle
	
	// Turn off interrupt
	TIMSK2 &= ~(1 << OCIE2B);
	
	// Turn off hour handle
	LED2_PORT &= ~(1 << PLED2);
}

ISR(TIMER1_COMPA_vect)
{
	// This interrupt fires every seconds
	
	// Update seconds, minutes hours
	seconds++;

	// Handle roll overs
	if (seconds >= 60) {
		seconds = 0;
		minutes++;
		
		if (minutes >= 60) {
			minutes = 0;
			hours++;
			
			if (hours >= 12) {
				hours = 0;
				state = UPDATE_TIME_FROM_RTC;
			}
		}
		
		five_minutes = minutes / 12;
		one_minutes = minutes % 12;
		minute_timer_counts = sensor_counts + one_minutes * minute_counts;
	}
	
	// TODO: check if we need to adjust handle position in this function too
}

ISR(PCINT0_vect)
{
	// This interrupt fires when the button on the board is pressed
	
	// Other pins of this port might fire this interrupt. Check for button pin
	uint8_t button_state = get_button_state();
	if (button_state != default_button_state) {
		// Do something
	}
}