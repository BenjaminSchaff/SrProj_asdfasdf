#include "ui.h"

SCREEN *update_screen_state(int button, int current_screen_index, SCREEN *current_screen)
{
	int ret = current_screen_index;

	if (button == 1) { // if the right arrow key is pressed
		current_screen->line_values[current_screen->curser_index]++; // increment value of line
		
		// check if the line value is overflowed
		if (current_screen->line_values[current_screen->curser_index] >= current_screen->max_values[current_screen->curser_index])
			current_screen->line_values[current_screen->curser_index] = 0; // loop back to 0

	} else if (button == 5) { // if the left arrow key is pressed
		current_screen->line_values[current_screen->curser_index]--; // decrement value of line

		// check if the line value goes below 0
		if (current_screen->line_values[current_screen->curser_index] < 0)
			current_screen->line_values[current_screen->curser_index] = current_screen->max_values[current_screen->curser_index];

	} else if (button == 2) { // if the up arrow key is pressed
		if (current_screen->screen_index == current_screen->curser_index) { // if the curser is at the top of the screen
			if (current_screen->curser_index != 0) { // if the curser is not
				current_screen->screen_index--; // decrement both the curser and screen index
				current_screen->curser_index--;
			}
		} else { // if the curser is not at the top of the screen
			current_screen->curser_index--; // decrement the curser index
		}
	} else if (button == 4) { // if the down key is pressed
		if ((current_screen->screen_index + 3) == current_screen->curser_index) { // if the curser is at the bottom of the screen
			if (current_screen->curser_index != (current_screen->length + 1)) { // if the curser has not hit the bottom of the list
				current_screen->screen_index++; // increment screen and curser index
				current_screen->curser_index++;
			}
		} else {
			current_screen->curser_index++; // increment curser index
		}
	} 
}

void update_screen_text(SCREEN *current_screen)
{

}
