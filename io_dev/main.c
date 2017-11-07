#include "defines.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdint.h>

#include "uart.h"
#include "i2c.h"

FILE uart_strm = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

uint16_t temp, humid;
uint16_t c_temp, c_humid; // in dec-celcius and 


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
	// read temp and humidity
	i2c_start();
	i2c_write_byte((TEMP_SENSE_ADDR<<1) | 0x01);
	temp = (i2c_read_byte(1) << 8);	// temp MSB
	temp |= i2c_read_byte(1);	// temp LSB

	humid = (i2c_read_byte(1) << 8);	// humid MSB
	humid |= i2c_read_byte(0);	// humid LSB
	i2c_stop();

	c_temp = ((uint32_t)(temp) * 1650) / 65536 - 400;
	c_humid = ((uint32_t)(humid) * 1000) / 65536;

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

		//fprintf(&uart_strm, "Temperature: %f C\n", f_temp);
		fprintf(&uart_strm, "T: %d.%dC\n", c_temp/10, c_temp%10);
		fprintf(&uart_strm, "H: %d.%d%%\n", c_humid/10, c_humid%10);
//		fprintf(&uart_strm, "%f%%\n", f_humid);
	}
}
