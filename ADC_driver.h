#ifndef _ADC_DRIVER_H_
#define _ADC_DRIVER_H_

#include <stdint.h>

#define ADC_CH_MIN			5		// Using channel 5
#define ADC_CH_HOUR			4		// Using channel 4
#define ADC_ADPS		0b111	// Clock prescaler is 128
#define ADC_DATA_MAX	1023	// Maximum of ADC result (2^10 - 1)

void ADC_init(void);
uint16_t ADC_read(uint8_t channel);
uint16_t ADC_read_avg(uint8_t channel, uint8_t sample_size);

#endif /* _ADC_DRIVER_H_ */