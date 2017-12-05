#include <stdio.h>

void button_init(void)
{
	DDRC &= 0x03; // makes pins PC2-7 inputs
}

uint8_t button_pressed(void)
{
	if (PINC & (1 << PC2) == 1) // switch S1 is pressed (right)
		return 1;
	else if (PINC & (1 << PC3) == 1) // switch S2 is pressed (up)
		return 2;
	else if (PINC & (1 << PC4) == 1) // switch S3 is pressed (b1)
		return 3;
	else if (PINC & (1 << PC5) == 1) // switch S4 is pressed (down)
		return 4;
	else if (PINC & (1 << PC6) == 1) // switch S5 is pressed (left)
		return 5;
	else if (PINC & (1 << PC7) == 1) // switch S6 is pressed (b2)
		return 6;

	return 0;
}
