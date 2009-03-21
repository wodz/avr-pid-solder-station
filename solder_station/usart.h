#ifdef USART_U2X
#define UBRRH_VALUE ((((uint32_t)(F_CPU) / ((uint32_t)BAUD * 8UL) - 1)>>8) & 0xFF)
#define UBRRL_VALUE (((uint32_t)(F_CPU) / ((uint32_t)BAUD * 8UL) - 1) & 0xFF )
#else
#define UBRRH_VALUE ((((uint32_t)((F_CPU) ) / ((uint32_t)BAUD * 16UL) - 1) >> 8) & 0xFF)
#define UBRRL_VALUE (((uint32_t)((F_CPU) ) / ((uint32_t)BAUD * 16UL) - 1) & 0xFF )
#endif

//ring buffers have to be 2^n bytes in size (max 128 as counters are declared as uint8_t)
#define TXBUFSIZE	32
#define RXBUFSIZE	32
#define TXMASK		(TXBUFSIZE-1)
#define RXMASK		(RXBUFSIZE-1)

volatile unsigned char TxBuf[TXBUFSIZE];	//transmit buffer
volatile unsigned char RxBuf[RXBUFSIZE];	//receive buffer

volatile uint8_t tx_end;
volatile uint8_t tx_start;

volatile uint8_t rx_end;
volatile uint8_t rx_start;

int USART_putchar(char c, FILE *fp);
int USART_getchar(FILE *fp);

void USART_init(void);

