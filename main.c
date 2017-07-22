#include "time_meas.h"
#include "UART_driver.h"
#include "ADC_driver.h"
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/delay.h>

#define LED_DDR			DDRB
#define LED_DDR_POS		DDRB5
#define LED_PIN			PINB
#define LED_PIN_POS		PINB5
#define LED_PORT		PORTB
#define LED_PORT_POS	PORTB5
#define CNTR_MAX		10

#define BUTTON_PIN				PINB7
#define BUTTON_PIN_REG 			PINB
#define BUTTON_PIN_INT			PCINT7
#define BUTTON_PIN_INT_GROUP	PCIE0

#define ON		1
#define OFF		0

void init_pins(void);
uint8_t button_init(void);
void set_time(void);

volatile uint8_t state = OFF;


void system_init()
{
	LED_DDR |= 1 << LED_DDR_POS;
	time_meas_init();
	button_init();
	ticks_init();
	init_pins();
	UART_init();
	ADC_init();
	
	sei();
}

int main(void)
{

	// Don't forget to call the init function :)
	system_init();

	// Setting up STDIO input and output buffer
	// You don't have to understand this!
	//----- START OF STDIO IO BUFFER SETUP
	FILE UART_output = FDEV_SETUP_STREAM((void *)UART_send_character, NULL, _FDEV_SETUP_WRITE);
	stdout = &UART_output;
	FILE UART_input = FDEV_SETUP_STREAM(NULL, (void *)UART_get_character, _FDEV_SETUP_READ);
	stdin = &UART_input;
	//----- END OF STDIO IO BUFFER SETUP

	// Try printf
	printf("Startup...\r\n");

	// Infinite loop
	while (1) {
		//set time
		set_time();
		//Print hours and minutes
		printf("h: %d\tm: %d\n", hours, minutes);
	}
}


void init_pins(void)
{
	// Inputs
	// PD2, PD6, PD7, PB7, PC4, PC5
	DDRD = 0;
	DDRB = 0;
	DDRC = 0;

	// Outputs
	DDRD |= (1 << DDRD6) | (1 << DDRD7);
	DDRB |= (1 << DDRB5);
	
	return;
}

// Start button settings
uint8_t button_init(void)
{
	// Enable interrupts on button pin
	PCMSK0 |= 1 << BUTTON_PIN_INT;

	// Enable interrupts on button pin group
	PCICR |=  1 << BUTTON_PIN_INT_GROUP;

	return 0;
}

void set_time(void)
{
	if (state == OFF) {
		hours = (uint8_t) (ADC_read(ADC_CH_HOUR) / 93);
		if (hours > 11)
		hours = 11;
		
		minutes = (uint8_t) (ADC_read(ADC_CH_MIN) / 17);
		if (minutes > 59)
		minutes = 59;
	}
}
