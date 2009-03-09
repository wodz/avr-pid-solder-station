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
	static int16_t integral;
	int16_t derivative;
	int32_t result;

	errors[index & MASK] = setpoint - temperature;

	integral += errors[index];

	if (integral > INTEGRAL_MAX)
		integral = INTEGRAL_MAX;
	else if (integral < INTEGRAL_MIN)
		integral = INTEGRAL_MIN;

	derivative = errors[index & MASK] - errors[(index-1) & MASK];

	// units of 1/16
	result = (pid_setup->KP*(int32_t)errors[index & MASK] + pid_setup->KI*(int32_t)integral + pid_setup->KD*(int32_t)derivative)/16;

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
