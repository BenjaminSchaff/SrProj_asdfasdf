/*!
 * @file ui.h
 *
 * @author Dan Paradis and Ben Schaff
 *
 * Functions to setup a basic user interface for showing sensor readigns
 * and allowing the user to change settings such as units or values displayed.
 *
 */

#ifndef asdfasdf_ui
#define asdfasdf_ui

// Struct to store the state of each screen in the UI
typedef struct screen_ui {
	char lines[7][20];
	int line_values[7];
	int max_values[7];
	int length;
	int screen_index;
	int cursor_index;
} SCREEN;

/*
 * Saves the settings values to EEPROM
 */
void store_settings(SCREEN *settings);

/*!
 * Updates the LCD with the current screen.
 */
void print_screen(SCREEN *current_screen);

/*!
 * Updates the state of the screen based on current screen and button input.
 */
void update_screen_state(int button, int current_screen_index, SCREEN *current_screen);

/*!
 * Updates the settings screen strings based on current settings.
 */
void update_settings_strings(SCREEN *s);

/*!
 * Updates the sensors screen strings based on current measurements.
 */
void update_sensor_strings(SCREEN *sensors, SCREEN *settings, char ret[21]);

/*!
 * Initializes UI structs for main menu, settings, and sensors screens.
 */
void ui_init(SCREEN *ui);

/*!
 * The functions return the strings that should be saved in the sensor
 * screens' lines arrays using the current measurements.
 */
char *get_temp_string(SCREEN *settings, char ret[21]);
char *get_pressure_string(SCREEN *settings, char ret[21]);
char *get_wind_string(SCREEN *settings, char ret[21]);
char *get_humidity_string(char ret[21]);
char *get_wind_chill_string(char ret[21]);
char *get_dew_point_string(char ret[21]);
char *get_humidex_string(char ret[21]);

#endif
