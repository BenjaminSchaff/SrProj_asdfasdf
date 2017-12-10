#ifndef asdfasdf_ui
#define asdfasdf_ui

typedef struct screen_ui {
	char lines[7][20];
	int line_values[7];
	int max_values[7];
	int length;
	int screen_index;
	int curser_index;
} SCREEN;

void print_screen(SCREEN *current_screen);
void update_screen_state(int button, int current_screen_index, SCREEN *current_screen);
void update_screen_text(SCREEN *current_screen);
void update_settings_strings(SCREEN *s);
void update_sensor_strings(SCREEN *sensors, SCREEN *settings, char ret[21]);

void ui_init(SCREEN *ui);

char *get_temp_string(SCREEN *settings, char ret[21]);
char *get_pressure_string(SCREEN *settings, char ret[21]);
char *get_wind_string(SCREEN *settings, char ret[21]);
char *get_humidity_string(char ret[21]);
char *get_wind_chill_string(char ret[21]);
char *get_dew_point_string(char ret[21]);
char *get_humidex_string(char ret[21]);

#endif
