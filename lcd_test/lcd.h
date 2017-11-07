#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

struct bits {
  uint8_t b0, b1, b2, b3, b4, b5, b6, b7;
} __attribute__((__packed__));
#define SBIT_(port,pin) ((*(volatile struct bits*)&port).b##pin)
#define SBIT(x,y)       SBIT_(x,y)

#define LCD_D4          SBIT(PORTD, 3)
#define LCD_DDR_D4      SBIT(DDRD, 3)

#define LCD_D5          SBIT(PORTD, 4)
#define LCD_DDR_D5      SBIT(DDRD, 4)

#define LCD_D6          SBIT(PORTD, 5)
#define LCD_DDR_D6      SBIT(DDRD, 5)

#define LCD_D7          SBIT(PORTD, 6)
#define LCD_DDR_D7      SBIT(DDRD, 6)

#define LCD_RS          SBIT(PORTA, 6)
#define LCD_DDR_RS      SBIT(DDRA, 6)

#define LCD_E0          SBIT(PORTA, 8)
#define LCD_DDR_E0      SBIT(DDRA, 8)

static void lcd_nibble(uint8_t d);
static void lcd_byte(uint8_t d);
void lcd_command(uint8_t d);
void lcd_putchar(uint8_t d);
void lcd_init(void);
