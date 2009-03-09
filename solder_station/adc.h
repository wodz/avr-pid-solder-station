/*
 * adc.h
 *
 *  Created on: 2008-11-28
 *      Author: wodz
 */

#ifndef ADC_H_
#define ADC_H_

#define VREF_EXTERNAL	0
#define VREF_AVCC		1
#define VREF_INTERNAL	2

#define LEFT_JUSTIFY	1
#define RIGHT_JUSTIFY	0

#define ADC_FREE_RUN	1
#define ADC_ONE_SHOT	0

#define ENABLE_ADC_INTERRUPT	1
#define DISABLE_ADC_INTERRUPT	0

//void adc_init(uint8_t vref, uint8_t prescaler, uint8_t left_justify, uint8_t adc_free_run, uint8_t adc_interrupt);
//void adc_select_channel(uint8_t channel);
//uint16_t adc_read10(void);
//uint16_t adc_read9_averaged(void);
uint8_t adc_read8(void);
#endif /* ADC_H_ */
