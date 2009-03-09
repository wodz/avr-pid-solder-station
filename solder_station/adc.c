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

/*
void adc_init(uint8_t vref, uint8_t prescaler, uint8_t left_justify, uint8_t adc_free_run, uint8_t adc_interrupt)
{
// ADC voltage reference

switch (vref)
{
case 0:
	// external reference connected to AREF pin
	ADMUX &= ~((1<<REFS1)|(1<<REFS0));
	break;
case 1:
	// internal connection to AVcc AREF pin decoupled with 100n to GND
	ADMUX &= ~(1<<REFS1);
	ADMUX |= (1<<REFS0);
	break;
case 2:
	// internal 2.56V voltage reference AREF pin decoupled with 100n to GND
	ADMUX |= (1<<REFS1)|(1<<REFS0);
	break;
default:
	// external reference connected to AREF pin
	ADMUX &= ~((1<<REFS1)|(1<<REFS0));
	break;
}


// ADC prescaler

switch (prescaler)
{
case 2:
	ADCSRA &= ~((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));
	break;
case 4:
	ADCSRA &= ~((1<<ADPS2)|(1<<ADPS0));
	ADCSRA |= (1<<ADPS1);
	break;
case 8:
	ADCSRA &= ~(1<<ADPS2);
	ADCSRA |= (1<<ADPS1)|(1<<ADPS0);
	break;
case 16:
	ADCSRA &= ~((1<<ADPS1)|(1<ADPS0));
	ADCSRA |= (1<<ADPS2);
	break;
case 32:
	ADCSRA &= ~(1<<ADPS1);
	ADCSRA |= (1<<ADPS2)|(1<ADPS0);
	break;
case 64:
	ADCSRA &= ~(1<<ADPS0);
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1);
	break;
case 128:
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	break;
default:
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	break;
}

// ADC result placement

// left_justify = 0
// 15 14 13 12 11 10 9  8   ADCH
//  -  -  -  -  -  - 9  8   ADC bit
//
// 7  6  5  4  3  2  1  0   ADCL
// 7  6  5  4  3  2  1  0   ADC bit
//
//
// left_justyfy = 1
// 15 14 13 12 11 10 9  8    ADCH
// 9  8  7  6  5  4  3  2    ADC bit
//
// 7  6  5  4  3  2  1  0   ADCL
// 1  0  -  -  -  -  -  -   ADC bit
// ADCH has to be read last!!!

if (left_justify)
{
	ADMUX |= (1<<ADLAR);
}
else
{
	ADMUX &= ~(1<<ADLAR);
}

// ADC free run
if (adc_free_run)
{
	ADCSRA |= (1<<ADFR);
}
else
{
	ADCSRA &= ~(1<<ADFR);
}


// ADC interrupt
if (adc_interrupt)
{
	ADCSRA |= (1<<ADIE);
}
else
{
	ADCSRA &= ~(1<<ADIE);
}
}

void adc_select_channel(uint8_t channel)
{
	if (channel > 7)
		return;
// MUX Channel
ADMUX = (uint8_t)((ADMUX & 0xF8)|channel);
}


uint16_t adc_read10(void)
{
// result is: ADC = (Vin*1024)/Vref and is in range 0x000-0x3FF
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);

	// wait to complete
	loop_until_bit_is_set(ADCSRA, ADIF);

	ADCSRA |= (1<<ADIF); //clear flag
	ADCSRA &= ~(1<<ADEN); //disable adc for powersafe reasons

	return (uint8_t)(((uint16_t)ADCL|((uint16_t)ADCH<<8))&0x3FF);
}
*/

uint8_t adc_read8(void)
{
// result is: ADC = (Vin*1024)/(4*Vref) and is in range 0x00-0xFF
	// if 8bit resolution is enough set left_justify flag in adc_init and use this function
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);

	//wait to complete
	loop_until_bit_is_set(ADCSRA, ADIF);

	ADCSRA |= (1<<ADIF); //clear flag;
	ADCSRA &= ~(1<<ADEN); //disable adc for powersafe reasons

	return (uint8_t)ADCH;
}
/*
// private function
static uint16_t adc_read9(void)
{
	// 10bit signal cuted down to 8bit oversampled
	// and virtually bumped to 9bit resolution with averaging
	uint16_t adc;

	// take four 8bit samples
	adc = adc_read8() + adc_read8() + adc_read8() + adc_read8();
	// right shift by one giving us 9bit effective signal
	adc >>= 1;
	return (uint16_t)adc;
}

uint16_t adc_read9_averaged(void)
{
	static moving_average_struct adc_filter_struct;
	return moving_average(&adc_filter_struct,adc_read9(),8);

}
*/
