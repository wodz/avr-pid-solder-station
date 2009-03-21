/*
 * hardware.h
 *
 *  Created on: 2009-03-21
 *      Author: wodz
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

/* SPI */
#define SPI_PORT PORTB
#define SPI_PORT_DIR DDRB
#define SPI_SCK PB5
#define SPI_MISO PB4
#define SPI_MOSI PB3

/* USART */
#define USART_U2X
#define BAUD 9600UL

/* 3x 7segment display */
#define LED_CS PB2
#define LED_CS_PORT PORTB
#define LED_CS_PORT_DIR DDRB

/* MAX6675 temperature sensor */
#define MAX6675_CS_PORT		PORTB
#define MAX6675_CS			PB0
#define MAX6675_CS_PORT_DIR	DDRB

#endif /* HARDWARE_H_ */
