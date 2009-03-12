/*
 * isr.c
 *
 *  Created on: 2008-11-28
 *      Author: wodz
 */
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <stdint.h>

#include "moving_average.h"
#include "led_7segment.h"
#include "adc.h"
#include "max6675.h"
#include "pid.h"

#define POWER_SCALE	((PID_TOP/3)+1)
#define MAX_SETPOINT ((350/4)+1)
#define MAX_ADC_READOUT ((511/4)+1)
#define SETPOINT_OFFSET 0

volatile uint16_t temperature;				// temperature value read from max6675
volatile uint16_t setpoint;					// setpoint value read from adc
volatile uint16_t power;					// regulator output power
volatile uint8_t display_setpoint;			// setpoint display delay timer
//volatile uint8_t thermocouple_ok;			// thermocouple connected - unused

extern uint8_t log_enable;					// should we send log to RS232?
extern pid_t pid_s;							// pid struct

ISR(TIMER0_OVF_vect)
{
// read the setpoint from adc

	static movingaverage_t filter_s;
	uint16_t setpoint_read;

// read 8bit, oversample to get 9bit value than apply moving_average 'low pass' filter
// and finally scale up result to range from 100 to 450

	setpoint_read = (adc_read8() + adc_read8() + adc_read8() + adc_read8());
	setpoint_read >>= 1;
	setpoint_read = moving_average(&filter_s,setpoint_read,8);
	//setpoint_read = ((setpoint_read*88)/128)+100;	//scale up result and avoid overflow
	setpoint_read = (((setpoint_read*MAX_SETPOINT)/MAX_ADC_READOUT)+SETPOINT_OFFSET);


	if (setpoint != setpoint_read)
	{
		// setpoint has been changed
		display_setpoint = 50; // hold display of setpoint for about 5s
		setpoint = (uint16_t)setpoint_read;
		led_print(setpoint,0);
	}

	if (display_setpoint)
	{
		display_setpoint--;

		if (! display_setpoint)
		{
			// setpoint display timeout back to displaying temperature
			if (power)
			{
				led_print(temperature,(power/POWER_SCALE)+1);
			}
			else
			{
				led_print(temperature,0);
			}
		}
	}

}

ISR(TIMER2_OVF_vect)
{
	uint16_t temperature_read;
	static uint8_t slow_down;
	static movingaverage_t filter_struct;

	// read temperature from thermocouple
	// smooth readings with moving average filter

	if (slow_down == 0)
	{
		temperature_read = read_temperature();
		slow_down = 15;

	// leave out fraction part from readings
	temperature_read = moving_average(&filter_struct,temperature_read,8)>>2;
	power = pid(setpoint,temperature,&pid_s);
	OCR1A = power;	//set PWM fill factor
	}
	else
	{
		slow_down--;
		return;
	}


	if (temperature != temperature_read)
	{
		temperature = temperature_read;

		if (! display_setpoint)
		{
			// if we do not display setpoint currently, update temperature display

			// number indicate current temperature
			// dots indicate applied power
			if (power)
			{
				led_print(temperature,(power/POWER_SCALE)+1);
			}
			else
			{
				led_print(temperature,0);
			}
		}
	}



	if (log_enable)
		printf_P(PSTR("%d\t%d\t%d\n"),setpoint,temperature,power);

}

