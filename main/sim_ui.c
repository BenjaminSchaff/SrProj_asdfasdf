#include "defines.h"

#include <stdlib.h>
#include <stdio.h>

#include "sensors.h"
#include "ui.h"

void debug_print(SCREEN *current_screen)
{
	int i;

	for (i = 0; i < 4; i++) {
		if ((i + current_screen->screen_index) == current_screen->cursor_index)
			printf("%s <-\n", current_screen->lines[i + current_screen->screen_index]);
		else
			printf("%s   \n", current_screen->lines[i + current_screen->screen_index]);
	}
}

int main()
{
	char buf[21];

	SCREEN ui[3];

	ui_init(ui); // init
	debug_print(&ui[0]);
	
	return 0;
}
