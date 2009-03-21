/*
 * main.c
 *
 *  Created on: 2008-11-27
 *      Author: wodz
 *
 * Serial console 9600 8N1
 * available commands:
 *
 * set <parameter> <value>
 * log <0|1>
 * config
 * save
 *
 * set - sets value of parameter. Available parameters are KP, KI, KD
 *
 * log - enable (1) or disable (0) logging
 *
 * config - show currently running config (this can be different from saved config)
 *
 * eeprom - reads settings stored in eeprom
 *
 * save - store config in eeprom
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include <stdint.h>
#include <stdio.h>

#include "hardware.h"
#include "spi.h"
#include "adc.h"
#include "led_7segment.h"
#include "max6675.h"
#include "usart.h"
#include "pid.h"

#define LINE_SIZE	32				//line buffer size
#define NOP() __asm__ __volatile__ ("nop")

// PID constants stored in eeprom
uint8_t EEMEM EEMEM_KP;
uint8_t EEMEM EEMEM_KI;
uint8_t EEMEM EEMEM_KD;

volatile pid_t pid_s;
volatile uint8_t log_enable;

int main(void)
{
	const char *OK = PSTR("OK\n>");
	const char *PARAM_RANGE_MSG = PSTR("Out of range 0-%d\n>");
	const char *STATUS = PSTR("Solder station WO-1\n\tKP %d\n\tKI %d\n\tKD %d\n");
	char line[LINE_SIZE]; //line buffer
	int16_t tmp;

	cli();

// HARDWARE INITIALIZATION

// SPI

	//Initialize SPI Mode 0, MSB first, clk/128
	SPI_PORT_DIR |= (1<<SPI_SCK)|(1<<SPI_MOSI);
	SPI_PORT_DIR &= ~(1<<SPI_MISO);
	SPCR |= (1<<SPR1)|(1<<SPR0)|(1<<SPE)|(1<<MSTR); // clk/128 master mode

// CHIP SELECTS

	// setup of chained '595 driving 3x 7segment led display
	LED_CS_PORT_DIR |= (1<<LED_CS);

	LED_CS_PORT &= ~(1<<LED_CS);   // drive latch strobe low
	LED_CS_PORT ^= (1<<LED_CS);		// drive latch strobe high
	NOP();
	NOP();
	NOP();
	NOP();
	LED_CS_PORT ^= (1<<LED_CS);		//drive latch strobe low

	// setup temperature sensor chip
	MAX6675_CS_PORT_DIR |= (1<<MAX6675_CS);
	MAX6675_CS_PORT |= (1<<MAX6675_CS);

// ADC

	// ADC clock is recommended to be 50-200kHz (it is now 62.5 kHz)
	// Initialize ADC Vref = AVCC, data left justify, one shot mode, no interrupt, clk/128
	ADMUX &= ~((1<<REFS1)|(1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));	// Vref AVCC, data left justify, channel 0
	ADMUX |= (1<<REFS0)|(1<<ADLAR);

	ADCSRA |= (1<<ADPS2);				// 16 prescaler
	ADCSRA &= ~((1<<ADFR)|(1<<ADIE));	// one shot mode, no interrupt

// TIMERS

	// read setpoint
	//Timer0 upcounting clk/256 interrupt enabled
	TCCR0 |= (1<<CS02);		// clk/256
	TIMSK |= (1<<TOIE0);	// enable overflow interrupt

	// read temperature
	//Timer2 upcounting clk/1024 interrupt enabled
	TCCR2 |= (1<<CS22)|(1<<CS21)|(1<CS20);	// clk/1024
	TIMSK |= (1<<TOIE2);					// enable interrupt

	// Timer1 Phase correct PWM 9bit clk/64 ~15Hz with 1MHz clock
	// In this mode if OCR1A=0 we have truly 0 power applied
	TCCR1B |= (1<<CS11)|(1<<CS10);
	TCCR1A |= (1<<COM1A1)|(1<<WGM11);
	OCR1A = 0;
	DDRB |= (1<<PB1);

// USART

	// set baudrate
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;

#ifdef USART_2X
	UCSRA |= (1<<U2X);
#endif

	// enable receiver and transmitter
	// enable receiver interrupt
	// set frame format to 8bit and 1 stop bit
	UCSRB = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);

// GENERAL SETUP

	fdevopen(USART_putchar,USART_getchar);	//setup stdio streams

	// read pid parameters from eeprom

	pid_s.KP = eeprom_read_byte(&EEMEM_KP);
	pid_s.KI = eeprom_read_byte(&EEMEM_KI);
	pid_s.KD = eeprom_read_byte(&EEMEM_KD);

	if (pid_s.KP == 0xff && \
		pid_s.KI == 0xff && \
		pid_s.KD == 0xff)
	{
		//eeprom is uninitialized - let's fallback to some safe settings
		// here pure proportional behavior

		pid_s.KP = 1;
		pid_s.KI = 0;
		pid_s.KD = 0;

	}

	sei();	//Enable global interrupts

	led_print(888,3); // send something to '595

	// Be polite and introduce itself :-)
	printf_P(STATUS, \
			 pid_s.KP, \
			 pid_s.KI, \
			 pid_s.KD);
	printf_P(OK);

// MAIN LOOP

	// Main loop processes user input, everything else is run from ISRs
	while(1)
	{
		if (fgets(line,LINE_SIZE-1,stdin))
		{
			if (strncmp_P(line,PSTR("set KP "),7) == 0)
			{
				//set KP n
				sscanf_P(line,PSTR("set KP %d"),&tmp);

				if (tmp < MAX_PID_CONSTANT && tmp >= 0)
				{
					pid_s.KP = (uint8_t)tmp;
					printf_P(OK);
				}
				else
				{
					printf_P(PARAM_RANGE_MSG,MAX_PID_CONSTANT);
				}
			}

			else if (strncmp_P(line,PSTR("set KI "),7) == 0)
			{
				//set KI n
				sscanf_P(line,PSTR("set KI %d"),&tmp);

				if (tmp < MAX_PID_CONSTANT && tmp >= 0)
				{
					pid_s.KI = (uint8_t)tmp;
					printf_P(OK);
				}
				else
				{
					printf_P(PARAM_RANGE_MSG,MAX_PID_CONSTANT);
				}
			}

			else if (strncmp_P(line,PSTR("set KD "),7) == 0)
			{
				//set KD n
				sscanf_P(line,PSTR("set KD %d"),&tmp);

				if (tmp < MAX_PID_CONSTANT && tmp >= 0)
				{
					pid_s.KD = (uint8_t)tmp;
					printf_P(OK);
				}
				else
				{
					printf_P(PARAM_RANGE_MSG,MAX_PID_CONSTANT);
				}
			}

			else if (strncmp_P(line,PSTR("log "),3) == 0)
			{
				// log 0/1
				sscanf_P(line,PSTR("log %d"),&tmp);

				if (tmp == 0 || tmp == 1)
					log_enable = (uint8_t)tmp;

				if (log_enable)
					printf_P(PSTR("setpoint\ttemperature\tpower\n"));
				else
					printf_P(OK);
			}

			else if (strncmp_P(line,PSTR("save"),4) == 0)
			{
				cli();
				eeprom_busy_wait();
				eeprom_write_byte(&EEMEM_KP, pid_s.KP);
				eeprom_write_byte(&EEMEM_KI, pid_s.KI);
				eeprom_write_byte(&EEMEM_KD, pid_s.KD);
				sei();
				printf_P(OK);
			}
			else if (strncmp_P(line,PSTR("config"),6) == 0)
			{
				printf_P(STATUS, \
							 pid_s.KP,pid_s.KI,pid_s.KD);
				printf_P(OK);
			}
			else if (strncmp_P(line,PSTR("eeprom"),6) == 0)
			{
				printf("KP %d\nKI %d\nKD %d\n", \
						eeprom_read_byte(&EEMEM_KP), \
						eeprom_read_byte(&EEMEM_KI), \
						eeprom_read_byte(&EEMEM_KD));
				printf_P(OK);
			}
			else
			{
				printf_P(PSTR("?\n>"));
			}
		}
	}

	return 0;
}
