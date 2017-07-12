#include "ADC_driver.h"
#include <avr/io.h>
#include <stdint.h>

void ADC_init(void) {
	// TODO:
	// Set the reference voltage to AVcc.
	ADMUX |= 1 << REFS0;	

	// Set the data format to right adjusted, so the bottom 8bits will be in ADCL register
	// right adjust is the default setting

	// Set the used channel. Use the definitions from ADC_driver.h!
	// Channel is set from the read function
	
	// TODO:
	// Set the prescaler. Use the definitions from ADC_driver.h!
	ADCSRA |= ADC_ADPS;

	// TODO:
	// Enable the ADC peripheral
	ADCSRA |= 1 << ADEN;
}

uint16_t ADC_read(uint8_t channel) {
	
	// set the channel
	ADMUX &= ~(0x0f);
	ADMUX |= channel;
	
	// Start a conversion with ADSC bit setup
	ADCSRA |= 1 << ADSC;

	// TODO:
	// Wait for the conversion to finish by checking ADSC bit
	// ADSC is 1 until the measurement is running
	while(ADCSRA & (1 << ADSC));

	// TODO:
	// return with the read data, use the "ADC" register!
	return ADC;
}

uint16_t ADC_read_avg(uint8_t channel, uint8_t sample_size)
{
	uint16_t adc_averaged_value = 0;
	
	// Read ADC value sample times
	for (uint8_t i = 0; i < sample_size; i++) {
		adc_averaged_value += ADC_read(channel);
	}
	
	// Divide total read values by sample number
	adc_averaged_value /= sample_size;
	
	// Return average -> note it is still intiger
	return adc_averaged_value;
}