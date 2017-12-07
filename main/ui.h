#ifndef asdfasdf_ui
#define asdfasdf_ui

typedef struct screen_ui {
	char *lines;
	int *line_values;
	int *max_values;
	int length;
	int screen_index;
	int curser_index;
} SCREEN;

SCREEN *update_screen_state(int button, int current_screen_index, SCREEN *current_screen);
void update_screen_text(SCREEN *current_screen);
