#include "defines.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>


#include "hd44780_settings.h"
#include "hd44780.h"

int main()
{
	lcd_init();
	
  	while (1) {
		lcd_clrscr();
		lcd_home();
		lcd_puts("Hello World!");
		lcd_goto(0x40);
		lcd_puts("Second line");
		lcd_goto(0x14);
		lcd_puts("Third line");
		lcd_goto(0x54);
		lcd_puts("Fourth line");
		_delay_ms(50);
	}
}
