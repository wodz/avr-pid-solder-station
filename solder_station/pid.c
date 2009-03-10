/*
 * pid.c
 *
 *  Created on: 2009-01-23
 *      Author: wodz
 */
#include <stdint.h>
#include <stdio.h>
#include "pid.h"

uint16_t pid(uint16_t setpoint, uint16_t temperature, pid_t *pid_s)
{
	int32_t result;

	// calculate regulator error
	pid_s->errors[pid_s->index & MASK] = setpoint - temperature;

	// integrate error
	pid_s->integral += pid_s->errors[pid_s->index];

	// narrow down integral to prevent windup
	if (pid_s->integral > INTEGRAL_MAX)
	{
		pid_s->integral = INTEGRAL_MAX;
	}
	else if (pid_s->integral < INTEGRAL_MIN)
	{
		pid_s->integral = INTEGRAL_MIN;
	}

	// calculate derivative term
	pid_s->derivative = pid_s->errors[pid_s->index & MASK] - pid_s->errors[(pid_s->index-1) & MASK];

	// PID function
	result = ((int32_t)pid_s->KP*(int32_t)pid_s->errors[pid_s->index & MASK] + ((int32_t)pid_s->KI*(int32_t)pid_s->integral)/100 + (int32_t)pid_s->KD*(int32_t)pid_s->derivative);

	pid_s->index++;

	// regulator function bandwidth set to PID_TOP - PID_BOTTOM
	if (result > PID_TOP)
	{
		return (uint16_t)PID_TOP;
	}
	else if (result < PID_BOTTOM)
	{
		return (uint16_t)PID_BOTTOM;
	}
	else
	{
		return (uint16_t)result;
	}


}
