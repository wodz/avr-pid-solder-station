/*
 * pid.c
 *
 *  Created on: 2009-01-23
 *      Author: wodz
 */
#include <stdint.h>
#include <stdio.h>
#include "pid.h"

uint16_t pid(uint16_t setpoint, uint16_t temperature, pid_struct *pid_setup)
{
	static uint8_t index;
	static int16_t errors[NUMBER_OF_SAMPLES];
	int16_t integral = 0;
	int16_t derivative;
	int32_t result;
	uint8_t i;

	errors[index & MASK] = setpoint - temperature;

	for (i=0; i < NUMBER_OF_SAMPLES; i++)
		integral += errors[i];

	if (integral > INTEGRAL_MAX)
		integral = INTEGRAL_MAX;
	else if (integral < INTEGRAL_MIN)
		integral = INTEGRAL_MIN;

	derivative = errors[index & MASK] - errors[(index-1) & MASK];

	// units of 1/16
	result = (pid_setup->KP*errors[index & MASK] + pid_setup->KI*integral + pid_setup->KD*derivative)/8;

	index++;

	// regulator function badwidth set to PWM_TOP
	if (result > PWM_TOP)
	{
		return PWM_TOP;
	}
	else if (result < 0)
	{
		return 0;
	}
	else
	{
		return result;
	}


}
