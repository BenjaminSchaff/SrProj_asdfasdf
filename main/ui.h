#ifndef asdfasdf_ui
#define asdfasdf_ui

typedef struct screen_ui {
	char lines[7][20];
	int *line_values;
	int *max_values;
	int length;
	int screen_index;
	int curser_index;
} SCREEN;

void update_screen_state(int button, int current_screen_index, SCREEN *current_screen);
void update_screen_text(SCREEN *current_screen);
void update_settings_strings(SCREEN *s);
void update_sensor_strings(SCREEN *sensors, SCREEN *settings);

void ui_init(SCREEN *ui);

void get_temp_string(SCREEN *settings, char ret[21]);
void get_pressure_string(SCREEN *settings, char ret[21]);
void get_wind_string(SCREEN *settings, char ret[21]);
#endif
