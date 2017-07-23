/*
 * hdd_clock.h
 *
 * Created: 2017-07-21 00:54:09
 * Author : Krisztian Stancz
 */ 

#ifndef __HDD_CLOCK_H_INCLUDED
#define __HDD_CLOCK_H_INCLUDED

#include <string.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "UART_driver.h"
#include "ADC_driver.h"
#include "ds1302.h"
#include "freq_meas.h"

/* IO PINS
 *
 * Sensor - SENSOR - PD2 - INT0
 * 2nd input for Sensor for measuring RPM - RPMSENSOR - PB0 - ICP1
 * LED Strip Color 1 - LED1 - PD6
 * LED Strip Color 2 - LED2 - PD7
 * Button - BUTTON - PB7 - PCINT7
 * On board LED - INFOLED - PB5
 * Potentiometer to set hour - HSET - PC4 - ADC_CH_HOUR - ADC CH4
 * Potentiometer to set minutes - MSET - PC5 - ADC_CH_MIN - ADC CH5
 * Real Time Clock CLK - SCLK - PC0
 * Real Time Clock DAT - IO  - PC1
 * Real Time Clock RST - CE   - PC2
 */

// Pin setup
// NOTE: Real Time Clock IO settings are in ds1302.h
// NOTE: RPM measurement IO settings are in freq_meas.h

#define PSENSOR		2
#define PRPMSENSOR	0
#define PLED1		6
#define PLED2		7
#define PBUTTON		7
#define PINFOLED	5
#define PHSET		4
#define PMSET		5

#define SENSOR_PORT		PORTD
#define RPMSENSOR_PORT	PORTB
#define LED1_PORT		PORTD
#define LED2_PORT		PORTD
#define BUTTON_PORT		PORTB
#define INFOLED_PORT	PORTB
#define HSET_PORT		PORTC
#define MSET_PORT		PORTC

#define SENSOR_DDR		DDRD
#define RPMSENSOR_DDR	DDRB
#define LED1_DDR		DDRD
#define LED2_DDR		DDRD
#define BUTTON_DDR		DDRB
#define INFOLED_DDR		DDRB
#define HSET_DDR		DDRC
#define MSET_DDR		DDRC

#define SENSOR_PIN		PIND
#define RPMSENSOR_PIN	PINB
#define LED1_PIN		PIND
#define LED2_PIN		PIND
#define BUTTON_PIN		PINB
#define INFOLED_PIN		PINB
#define HSET_PIN		PINC
#define MSET_PIN		PINC

#define SENSOR_INT			INT0;
#define BUTTON_INT			PCINT7
#define BUTTON_INT_GROUP	PCIE0

#define ADC_CH_HOUR			4
#define ADC_CH_MIN			5

#define HOUR_SCALE			93	// divider to get 12 digits on potentiometer
#define MIN_SCALE			17	// divider to get 60 digits on potentiometer

#define DEGREES_TO_ZERO		10	// position of senor to the next hour mark in degrees
#define HOURS_TO_ZERO		6	// number of hour marks between sensor and 12 o'clock
#define HANDLE_THICKNESS	2	// thickness of minute and hour handles in degrees
#define ERROR_THRESHOLD		5	// max tolerable RPM difference between measurements in %
#define ENCODER_MARKS		6	// number of black strips on disk

typedef enum {SHOW_TIME, UPDATE_TIME_FROM_RTC, UPDATE_TIME_FROM_ADC} state_t;

volatile uint8_t hours;
volatile uint8_t minutes;
volatile uint8_t five_minutes;
volatile uint8_t one_minutes;
volatile uint16_t seconds;
volatile uint8_t hour_count;
volatile uint8_t five_minutes_count;
volatile state_t state;
volatile uint8_t button_state;
volatile uint8_t sensor_counts;
volatile uint8_t thickness_counts;
volatile uint8_t minute_counts;
volatile uint8_t minute_timer_counts;

double hdd_period;
double hdd_period_error;

void init_sensor(void);
void init_color_leds(void);
void init_button(void);
void init_info_led(void);
void init_clock_adc(void);
void get_time_from_adc(void);
uint8_t get_button_state(void);
void wait_for_stable_spin(void);
void print_float(float number, uint8_t decimals);
void get_time_from_rtc(void);
void calculate_timers(void);
void init_timers(void);
void adjust_time(void);
void start_seconds_counter(void);
void stop_seconds_counter(void);

#endif // __HDD_CLOCK_H_INCLUDED