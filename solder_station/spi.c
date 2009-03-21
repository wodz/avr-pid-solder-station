/*
 * spi.c
 *
 *  Created on: 2008-11-26
 *      Author: wodz
 */
#include <avr/io.h>
#include <stdint.h>

#include "spi.h"

void spi_byte_send (uint8_t byte)
{
	SPDR = byte;
	loop_until_bit_is_set(SPSR,SPIF); // wait to complete
}

uint8_t spi_byte_receive (void)
{
	// interchange shift register with slave
	spi_byte_send(0x00);
	return SPDR;
}
