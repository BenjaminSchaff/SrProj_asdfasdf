#include "defines.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdint.h>

#include "uart.h"
#include "i2c.h"

FILE uart_strm = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

uint16_t c_temp, c_humid;
float f_temp, f_humid;

void i2c_write_test()
{
	i2c_start();
	
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | 0x00);
//	i2c_write_byte((PRES_SENSE_ADDR<<1) | 0x00);
//	i2c_write_byte((0x77<<1) | 0x00); //BMP180
	i2c_write_byte(0xFF);
	i2c_stop();
/*	
	if (val != 0)
		fprintf(&uart_strm, "n%x", val);
	else 
		fprintf(&uart_strm, "y\1");*/
}

void sensor_init()
{


}

void read_sensors()
{

	// read measurements
	i2c_start();
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | 0x01);
	c_temp &= 0xFF;
	c_temp |= (i2c_read_byte(1) << 8) & 0xF0;	// temp MSB
	c_temp |= i2c_read_byte(1) & 0x0F;	// temp LSB

	c_humid &= 0xFF;
	c_humid |= (i2c_read_byte(1) << 8) & 0xF0;	// humid MSB
	c_humid |= i2c_read_byte(0) & 0x0F;	// humid LSB
	i2c_stop();

	f_temp = (float)((c_temp * 165) / 65536 - 40);
	f_humid = (float)((c_humid * 100) / 65536);

	// read temp and humidity
	// trigger measurement
	i2c_start();
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | 0x00);
	i2c_write_byte(0x00);
	i2c_stop();
	// wait for measurment (2.5ms to 6.5ms depending on resolution)
	_delay_ms(7);	
	
}

void init()
{
	DDRA |= (1<<4); // set PA4 to output (LED blink)

	uart_init();
	i2c_init();
	sensor_init();
}


int main() 
{
	init();
	

	while (1) {
		PORTA |= (1<<4);
		_delay_ms(100);
		PORTA &= ~(1<<4);
		_delay_ms(100);

// i2c_write_test();
		read_sensors();

		fprintf(&uart_strm, "test\n");

		fprintf(&uart_strm, "%X", TWSR);
		fprintf(&uart_strm, "Temperature: %f C\n", f_temp);
		fprintf(&uart_strm, "Humidity: %f\%\n", f_humid);
	}
}
