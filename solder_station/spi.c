/*
 * spi.c
 *
 *  Created on: 2008-11-26
 *      Author: wodz
 */
#include <avr/io.h>
#include <stdint.h>
#include "spi.h"
/*
void spi_init(uint8_t spi_mode, uint8_t data_order, uint8_t clk_devider)
{
	// mode 0 clock low idle, sample on leading edge
	// mode 1 clock low idle, sample on trailing edge
	// mode 2 clock high idle, sample on leading edge
	// mode 3 clock high idle, sample on trailing edge



	switch (clk_devider)
	{
	case 4:
		SPCR &=~((1<<SPI2X)|(1<<SPR1)|(1<<SPR0));
		break;
	case 8:
		SPCR |= (1<<SPI2X)|(1<<SPR0);
		SPCR &= ~(1<<SPR1);
		break;
	case 16:
		SPCR &= ~((1<<SPI2X)|(1<<SPR1));
		SPCR |= (1<<SPR0);
		break;
	case 32:
		SPCR |= (1<<SPI2X)|(1<<SPR1);
		SPCR &= ~(1<<SPR0);
		break;
	case 64:
		SPCR |= (1<<SPI2X)|(1<<SPR1)|(1<<SPR0);
		break;
	case 128:
		SPCR |= (1<<SPR1)|(1<<SPR0);
		SPCR &= ~(1<<SPI2X);
		break;
	default:
		SPCR |= (1<<SPR1)|(1<<SPR0);
		SPCR &= ~(1<<SPI2X);
		break;
	}

	switch (spi_mode)
	{
	case 0:
		SPCR &= ~(1<<CPOL)|(1<<CPHA);
		break;
	case 1:
		SPCR &= ~(1<<CPOL);
		SPCR |= (1<<CPHA);
		break;
	case 2:
		SPCR |= (1<<CPOL);
		SPCR &= ~(1<<CPHA);
		break;
	case 3:
		SPCR |= (1<<CPOL)|(1<<CPHA);
		break;
	default:
		SPCR &= ~(1<<CPOL)|(1<<CPHA);
		break;
	}

	SPI_PORT_DIR |= (1<<SPI_SCK)|(1<<SPI_MOSI);
	SPI_PORT_DIR &= ~(1<<SPI_MISO);

	if (data_order)
	{
		//LSB first
		SPCR |= (1<<DORD);
	}
	else
	{
		//MSB first
		SPCR &= ~(1<<DORD);
	}

	SPCR |= (1<<SPE)|(1<<MSTR);
}
*/
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
