/*
 * max6675.h
 *
 *  Created on: 2008-11-30
 *      Author: wodz
 */

#ifndef MAX6675_H_
#define MAX6675_H_

#define MAX6675_CS_PORT		PORTB
#define MAX6675_CS			PB0
#define MAX6675_CS_PORT_DIR	DDRB

uint16_t read_temperature(void);
#endif /* MAX6675_H_ */
