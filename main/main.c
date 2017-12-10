#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "uart.h"
#include "ui.h"
#include "i2c.h"
#include "sensors.h"
#include "hd44780_settings.h"
#include "hd44780.h"

//FILE uart_strm = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

char lcd_buf[21];	// LCD write buffer

volatile uint8_t last_button_state = 0;		// For storing last state of the buttons
volatile uint8_t button_event = 0;			// Flags for button events needing to be processed
volatile uint8_t since_last_update = 0xFF;	// Flag signalling to update the sensors

// Interrupt for periodic events including querying button state and setting 
// update sensor/display flag (occurs every 32ms or so)
ISR(TIMER0_OVF_vect)
{
	uint8_t state = (PINC>>1) & 0x7E; // Shift and mask PC2-PC7 to SW1-SW7
	button_event |= (state^last_button_state)&last_button_state; 
	// lasti XOR current detects rising/falling edges on any buttons
	// AND with last narrows it to falling edges only
	// OR into event to set flag on and for buttons that were pressed.

	last_button_state = state; // Save current state to last state
	
	// Increment count of interrupts since last update, up until max val
	if (since_last_update < 0xFF)
		since_last_update++;
}

int main() 
{
	int current_screen_index = 0;
	int i;
	SCREEN ui[3];	// for storing the state of all ui screens

	/* Initialiation */
	
	// LED config
	DDRA |= (1<<4); // set PA4 to output (LED blink)
	
	// Pushbutton config
	MCUCR &= ~(1<<PUD); // disable pullup disable
	DDRC &= ~(0xFC);    // PC2-PC7 set to input
	PORTC |= (0xFC);    // PC2-PC7 pullup enabled
	// Disable JTAG so PORTC works.  Twice, just to be sure.
	MCUCR = (1<<JTD); 	
	MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);

	// Configuring Timer 0 (TC0) to periodically interrupt for periodically
	// updating pushbuttons, sensors, and UI
	SREG |= 0x80;	// enable global interrupt bit
	TCCR0A = 0x00;	// No output compare or waveform. Overflow flag set at MAX
	TCCR0B = 0x05;	// No output compare, no waveform.  Clock = clk/1024
	TIMSK0 |= (1<<TOIE0);	// Enable timer overflow interrupt

	// Call init routines of other subsystems
	lcd_init();
//	uart_init();
	i2c_init();
	sensor_init();
	ui_init(ui);
	
	/* End initialization */

	/* Main loop */
	while (1) {
		// Processing button input events
		for (i = 1; i < 7; i++) { // loop through buttons 1-6
			if (button_event & (1<<i)) { // if button was pressed
				button_event &= ~(1<<i); // clear event flag

				if (i == 6) { // if the button is back screen, go home
					current_screen_index = 0; // 0 is the home screen
				} else if (i == 3) { // if you press the goto screen button
					if (current_screen_index == 0) { // and are at home screen
						if (ui[0].cursor_index == 0) // and cursor at sensors 
							current_screen_index = 1; // goto sensors screen
						else if (ui[0].cursor_index == 2) // cursor at settings
							current_screen_index = 2; // goto settings
					}
				}
				// update screens with new values
				update_screen_state(i, current_screen_index, 
						&ui[current_screen_index]); 
				
				// If on settings screen, and l/r button pressed, save settings
				if ((current_screen_index == 2) && ((i == 1)||(i ==5))) {
					store_settings(&ui[2]);	// Stores settings to EEPROM
				}
			}
		}
		// Periodic updates
		if (since_last_update > 20) {	// update sensors/display
			since_last_update = 0;	// clear counter

			print_screen(&ui[current_screen_index]); // update screen
			update_sensors();	// Get new values from sensors

			// Update UI/display with new sensor data
			switch (current_screen_index) {
			case 1:
				update_sensor_strings(&ui[1], &ui[2], lcd_buf);
				break;
			case 2:
				update_settings_strings(&ui[2]);
				break;
			}
			PORTA ^= (1<<4); // Toggle LED every update cycle
		}			
	}
}
