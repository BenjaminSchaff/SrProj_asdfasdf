#ifndef asdfasdf_UART
#define asdfasdf_UART
/*
 * Perform UART startup initialization.
 */
void uart_init(void);

/*
 * Send one char to the UART.
 */
int uart_putchar(unsigned char c);

#endif
