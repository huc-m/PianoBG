#ifndef CONFIG_H
#define CONFIG_H

int read_config(char* tune_config_directory);
int read_flyid_config (fluid_settings_t* fluid_settings);
int read_tune_config (char* config_file, char* midi_file);

#endif // CONFIG_H
