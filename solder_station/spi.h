/*
 * spi.h
 *
 *  Created on: 2008-11-27
 *      Author: wodz
 */

#ifndef SPI_H_
#define SPI_H_

#define SPI_PORT PORTB
#define SPI_PORT_DIR DDRB
#define SPI_SCK PB5
#define SPI_MISO PB4
#define SPI_MOSI PB3

#define SPI_MODE_0	0
#define SPI_MODE_1	1
#define SPI_MODE_2	2
#define SPI_MODE_3	3

#define MSB_FIRST	0
#define LSB_FIRST	1

//void spi_init(uint8_t spi_mode, uint8_t data_order, uint8_t clk_devider);
void spi_byte_send (uint8_t byte);
uint8_t spi_byte_receive (void);

#endif /* SPI_H_ */
