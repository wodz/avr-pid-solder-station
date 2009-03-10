/*
 * pid.h
 *
 *  Created on: 2009-01-24
 *      Author: wodz
 */

#ifndef PID_H_
#define PID_H_

#define MASK 0x01
#define INTEGRAL_MAX 511
#define INTEGRAL_MIN -511
#define PID_TOP	511
#define PID_BOTTOM 0

typedef struct {
	uint8_t KP;					//proportional term constant 0-255
	uint8_t KI;					//integral term constant 0-255
	uint8_t KD;					//differential term constant 0-255
	uint8_t index;				//errors array index
	int16_t errors[2];			//errors array
	int16_t integral;			//integral of the error
	int16_t derivative;			//derivative of the error
} pid_t;

uint16_t pid(uint16_t setpoint, uint16_t temperature, pid_t *pid_setup);

#endif /* PID_H_ */
