#ifndef __ADC_DRIVER_H_INCLUDED
#define __ADC_DRIVER_H_INCLUDED

#include <stdint.h>
#include <avr/io.h>

#define ADC_ADPS		0b111	// Clock prescaler: 128
#define ADC_DATA_MAX	1023	// ADC resolution 10 bit

void ADC_init(void);
uint16_t ADC_read(uint8_t channel);
uint16_t ADC_read_avg(uint8_t channel, uint8_t sample_size);

#endif // __ADC_DRIVER_H_INCLUDED