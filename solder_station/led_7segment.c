/*
 * led_7segment.c
 *
 *  Created on: 2008-11-28
 *      Author: wodz
 */
#include <avr/io.h>
#include <stdint.h>

#include "hardware.h"
#include "spi.h"
#include "led_7segment.h"
/*   +-A-+			A=0x20 B=0x10 C=0x04 D=0x02 E=0x01 F=0x40 G=0x80 DOT=0x08
 *   |   |
 *   F   B
 *   |   |
 *   +-G-+
 *   |   |
 *   E   C
 *   |   |
 *   +-D-+  DOT
 *
 * pins are connected 1:1 to '595 latches and BCD-to-7seg transformation is kept in software
 */

static uint8_t bcd_7seg[] = { 0x77, 0x14, 0xB3, 0xB6, 0xD4, 0xE6, 0xE7, 0x34, 0xF7, 0xF6 };

void led_print(uint16_t number,uint8_t dots)
{
	uint8_t hundreds, tens, unity;
	uint8_t hundreds_dot,tens_dot,unity_dot;
	if (number > 999)
		// can not display value bigger than 999 using 3 digits
		return;

if (dots == 1 )
{
	unity_dot = 0x08;
	tens_dot = 0x00;
	hundreds_dot = 0x00;
}
else if (dots == 2)
{
	unity_dot = 0x08;
	tens_dot = 0x08;
	hundreds_dot = 0x00;
}
else if (dots == 3)
{
	unity_dot = 0x08;
	tens_dot = 0x08;
	hundreds_dot = 0x08;
}
else
{
	unity_dot = tens_dot = hundreds_dot = 0;
}

//BCD
	hundreds = number/100;
	tens = (number-(hundreds*100))/10;
	unity = number-(hundreds*100+tens*10);

	spi_byte_send(bcd_7seg[unity] | unity_dot);
	spi_byte_send(bcd_7seg[tens] | tens_dot);
	spi_byte_send(bcd_7seg[hundreds] | hundreds_dot);

	//latch data
	LED_CS_PORT ^= (1<<LED_CS);
	LED_CS_PORT ^= (1<<LED_CS);

}
