#include <stdlib.h>
#include <stdio.h>

#include "sensors.h"
#include "ui.h"

void debug_print(SCREEN *current_screen)
{
	int i;

	for (i = 0; i < 4; i++) {
		if ((i + current_screen->screen_index) == current_screen->curser_index)
			printf("%s <-\n", current_screen->lines[i + current_screen->screen_index]);
		else
			printf("%s   \n", current_screen->lines[i + current_screen->screen_index]);
	}
}

int main()
{
	char buf[21];
	int button, current_screen = 0;

	SCREEN ui[3];

	//printf("ASSSSS\n");
	ui_init(ui); // init
	//printf("ASSSSS\n");

	while (1) {
		debug_print(&ui[current_screen]);
		scanf("%d", &button); // take command 0-6
		if (button == 3)
			current_screen = 1;
		if (button == 6)
			current_screen = 2;
		if (button == 7)
			current_screen = 0;
		update_screen_state(button, current_screen, &ui[current_screen]);
		switch (current_screen) {
		case 1:
			update_sensor_strings(&ui[1], &ui[2], buf);
			break;
		case 2:
			update_settings_strings(&ui[2]);
			break;
		default:
			break;
		}
	}
	
	return 0;
}
