/*!
 * @file uart.h
 *
 * @author Dan Paradis and Ben Schaff
 *
 * Functions to init UART and write out characters to the bus.
 *
 */

#ifndef asdfasdf_UART
#define asdfasdf_UART

/*!
 * Perform UART startup initialization.
 */
void uart_init(void);

/*!
 * Send one char to the UART.
 */
int uart_putchar(unsigned char c);

#endif
