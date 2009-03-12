/*
 * pid.c
 *
 *  Created on: 2009-01-23
 *      Author: wodz
 */
#include <stdint.h>
#include <stdio.h>
#include "pid.h"

int16_t pid(uint16_t setpoint, uint16_t temperature, pid_t *pid_s)
{
	int32_t result;

	// calculate regulator error
	pid_s->errors[pid_s->index & MASK] = setpoint - temperature;

	// calculate derivative term
		pid_s->derivative = pid_s->errors[pid_s->index & MASK] - pid_s->errors[(pid_s->index-1) & MASK];

	// if we are far away from setpoint use PD otherwise use full PID function
	if (pid_s->errors[pid_s->index & MASK] > PID_THRESHOLD || pid_s->errors[pid_s->index & MASK] < -PID_THRESHOLD)
	{
		result = (pid_s->KP*pid_s->errors[pid_s->index & MASK] + pid_s->KD*pid_s->derivative);
	}
	else
	{
		// integrate error
		pid_s->integral += pid_s->errors[pid_s->index & MASK];

		// narrow down integral to prevent windup
		// probably this is not needed in current form of regulator function
		// but I does not make any harm

		if (pid_s->integral > INTEGRAL_MAX)
		{
			pid_s->integral = INTEGRAL_MAX;
		}
		else if (pid_s->integral < INTEGRAL_MIN)
		{
			pid_s->integral = INTEGRAL_MIN;
		}



		// PID function
		result = (pid_s->KP*pid_s->errors[pid_s->index & MASK] + (pid_s->KI*pid_s->integral)/100 + pid_s->KD*pid_s->derivative);
	}

	pid_s->index++;

	// regulator function bandwidth set to PID_TOP - PID_BOTTOM
	if (result > PID_TOP)
	{
		return (int16_t)PID_TOP;
	}
	else if (result < PID_BOTTOM)
	{
		return (int16_t)PID_BOTTOM;
	}
	else
	{
		return (int16_t)result;
	}


}
