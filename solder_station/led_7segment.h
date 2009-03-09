/*
 * led_7segment.h
 *
 *  Created on: 2008-11-28
 *      Author: wodz
 */

#ifndef LED_7SEGMENT_H_
#define LED_7SEGMENT_H_

#define LED_CS PB2
#define LED_CS_PORT PORTB
#define LED_CS_PORT_DIR DDRB

void led_print(uint16_t number, uint8_t dots);
void led_print_error(void);

#endif /* LED_7SEGMENT_H_ */
