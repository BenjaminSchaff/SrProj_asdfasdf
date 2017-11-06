#include "lcd.h"

static void lcd_nibble( uint8_t d )
{
	LCD_D4 = 0; 
	if (d & 1 << 4) 
		LCD_D4 = 1;

	LCD_D5 = 0; 
	if (d & 1 << 5) 
		LCD_D5 = 1;

	LCD_D6 = 0; 
	if (d & 1 << 6) 
		LCD_D6 = 1;

	LCD_D7 = 0; 
	if (d & 1 << 7) 
		LCD_D7 = 1;

	LCD_E0 = 1;
	_delay_us(1);                    
	LCD_E0 = 0;
}

static void lcd_byte(uint8_t d)
{
	lcd_nibble(d);
  	lcd_nibble(d << 4);
  	_delay_us(50);                    
}

void lcd_command(uint8_t d)
{
  	LCD_RS = 0;
  	lcd_byte(d);

  	if (d <= 3)
    	_delay_ms(2);                     // wait 2ms
}

void lcd_putchar(uint8_t d)
{
  	LCD_RS = 1;
  	lcd_byte(d);
}

void lcd_init()
{
  	LCD_DDR_D4 = 1;         
  	LCD_DDR_D5 = 1;
  	LCD_DDR_D6 = 1;
  	LCD_DDR_D7 = 1;
  	LCD_DDR_RS = 1;
  	LCD_DDR_E0 = 1;
  	LCD_E0 = 0;
  	LCD_RS = 0;             

  	_delay_ms(15);
  	lcd_nibble(0x30);
  	_delay_ms(4.1);
  	lcd_nibble(0x30);
  	_delay_us(100);
  	lcd_nibble(0x30);
  	_delay_us(LCD_TIME_DAT);
  	lcd_nibble(0x20);      
  	_delay_us(LCD_TIME_DAT);
  	lcd_command(0x28);                       
  	lcd_command(0x08);                        
  	lcd_command(0x01);                         
  	lcd_command(0x06);                          
  	lcd_command(0x0C);                          
}
