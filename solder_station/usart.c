#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "usart.h"

ISR(USART_RXC_vect)
{
	//got byte
	char c;
	c = UDR;

	// minimal processing to make stdio functions happy
	if (c == '\r')
		c = '\n';

	RxBuf[rx_end++ & RXMASK] = c;

}

ISR(USART_UDRE_vect)
{
	//send byte
	if(tx_end != tx_start)
	{
		UDR = TxBuf[tx_start++ & TXMASK];
	}
	else
	{
		//ring buffer empty
		UCSRB &= ~(1<<UDRIE);
	}
}

int USART_putchar(char c, FILE *fp)
{
	//wyslij znak,czekaj jesli bufor jest pelen
	while((tx_end-tx_start)>TXBUFSIZE-2);

		TxBuf[tx_end++ & TXMASK] = c;

	UCSRB |= (1<<UDRIE);			// Enable UDR empty interrupt

	return (uint8_t)c;
}

int USART_getchar(FILE *fp)
{

	while((rx_end - rx_start) == 0);

	return RxBuf[rx_start++ & RXMASK];
}

void USART_init(void)
{


}



