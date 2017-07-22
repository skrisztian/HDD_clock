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

#define HOUR_SCALE			93
#define MIN_SCALE			17

#define ON		1
#define OFF		0

#define ERROR_THRESHOLD		5	

volatile uint8_t hours;
volatile uint8_t minutes;
volatile uint8_t state;
volatile uint8_t button_state;
float hdd_period;


void init_sensor(void);
void init_color_leds(void);
void init_button(void);
void init_info_led(void);
void init_clock_adjusts(void);
void adjust_time(void);
uint8_t get_button_state(void);
void wait_for_stable_spin(void);
void get_time_from_rtc(void);



#endif // __HDD_CLOCK_H_INCLUDED