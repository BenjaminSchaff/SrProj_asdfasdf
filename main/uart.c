/*!
 * @file uart.c
 *
 * @author Dan Paradis and Ben Schaff
 *
 * Functions to init UART and write out characters to the bus.
 *
 */

#include "defines.h"

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include "uart.h"

/* UART baud rate */
#define UART_BAUD 9600

/*!
 * Initialize the UART to 9600 Bd, tx/rx, 8 bit, 1 stop.
 */
void uart_init(void)
{
	UBRR0L = (F_CPU / (16UL * UART_BAUD)) - 1;
  
	UCSR0B = (1<<TXEN0) | (1<<RXEN0); /* Enable RX and TX */
}

/*!
 * Send character c to UART
 */
int uart_putchar(unsigned char c)
{  
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));

	/* put char in tx buffer */
	UDR0 = c;

	return 0;
}
