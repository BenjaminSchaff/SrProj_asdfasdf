#include <stdio.h>
#include <avr/io.h>
#define F_CPU 2000000
#include <util/delay.h>

#include "lcd.h"

int main()
{
	lcd_init();
	lcd_xy(2, 0);
 	lcd_puts("Test");

  	while (1);
}
