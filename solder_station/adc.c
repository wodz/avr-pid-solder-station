/*
 * adc.c
 *
 *  Created on: 2008-11-28
 *      Author: wodz
 */
#include <avr/io.h>
#include <stdint.h>

#include "adc.h"
#include "moving_average.h"

uint8_t adc_read8(void)
{
// result is: ADC = (Vin*1024)/(4*Vref) and is in range 0x00-0xFF

	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);

	//wait to complete
	loop_until_bit_is_set(ADCSRA, ADIF);

	ADCSRA |= (1<<ADIF); //clear flag;
	ADCSRA &= ~(1<<ADEN); //disable adc for powersafe reasons

	return (uint8_t)ADCH;
}
