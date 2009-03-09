/*
 * max6675.c
 *
 *  Created on: 2008-11-30
 *      Author: wodz
 */
#include <avr/io.h>
#include <stdint.h>
#include "max6675.h"
#include "spi.h"

uint16_t read_temperature(void)
{
	uint16_t max6675_readout;

	// Chip select
	MAX6675_CS_PORT &= ~(1<<MAX6675_CS);

	max6675_readout = ( spi_byte_receive()<<8 ) | spi_byte_receive();

	MAX6675_CS_PORT |= (1<<MAX6675_CS);

		return (int16_t)(( max6675_readout>>3 ) & 0xFFF);

}
