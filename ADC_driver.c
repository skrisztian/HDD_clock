#include "ADC_driver.h"

void ADC_init(void) {

	// Set reference voltage to AVcc.
	ADMUX |= 1 << REFS0;	

	// Set pre-scaler.
	ADCSRA |= ADC_ADPS;

	// Enable the ADC peripheral
	ADCSRA |= 1 << ADEN;
	
	return;
}

void ADC_deinit(void) {

	// Disable the ADC peripheral
	ADCSRA &= ~(1 << ADEN);
	
	return;
}

uint16_t ADC_read(uint8_t channel) {
	
	// Clear then set ADC channel
	ADMUX &= ~(0x0f);
	ADMUX |= channel;
	
	// Start conversion
	ADCSRA |= 1 << ADSC;

	// Wait for the conversion to finish
	while(ADCSRA & (1 << ADSC));

	return ADC;
}

uint16_t ADC_read_avg(uint8_t channel, uint8_t samples)
{
	uint16_t adc_average = 0;
	
	// Read ADC value sample times
	for (uint8_t sample = 0; sample < samples; sample++) {
		adc_average += ADC_read(channel);
	}
	
	// Divide total read values by sample number
	adc_average /= samples;
	
	return adc_average;
}