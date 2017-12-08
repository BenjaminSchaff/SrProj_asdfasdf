#ifndef asdfasdf_ui
#define asdfasdf_ui

typedef struct screen_ui {
	char **lines;
	int *line_values;
	int *max_values;
	int length;
	int screen_index;
	int curser_index;
} SCREEN;

void update_screen_state(int button, int current_screen_index, SCREEN *current_screen);
void update_screen_text(SCREEN *current_screen);
void update_settings_strings(SCREEN *s);
void update_sensor_strings(SCREEN *s);
char *get_temp_string();
char *get_pressure_string();
char *get_wind_string();
