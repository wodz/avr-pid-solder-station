/*
 * pid.h
 *
 *  Created on: 2009-01-24
 *      Author: wodz
 */

#ifndef PID_H_
#define PID_H_

#define NUMBER_OF_SAMPLES 2  // size must be 2^n
#define MASK (NUMBER_OF_SAMPLES - 1)
#define INTEGRAL_MAX 511
#define INTEGRAL_MIN -511
#define PWM_TOP	511
typedef struct {
	uint8_t KP;					//proportional term constant 0-255
	uint8_t KI;					//integral term constant 0-255
	uint8_t KD;					//differential term constant 0-255
} pid_struct;

pid_struct pid_setup;

uint16_t pid(uint16_t setpoint, uint16_t temperature, pid_struct *pid_setup);

#endif /* PID_H_ */
