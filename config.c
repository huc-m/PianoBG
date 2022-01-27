#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <globals.h>
#include <stdbool.h>

int synth_channel_out = 0;
int wrong_note_key = 100;
int wrong_note_velocity = 50;
int guys_progress_breadth = 40;
int number_of_coming_notes = 20;


int read_config(char *tune_config_directory){
   FILE* config_file;
   char data[CONFIG_LINE_CHARACTER_MAX];
   char option[CONFIG_OPTION_CHARACTER_MAX];
   int value;
   bool key = false;

   sprintf(data, "%s%s", getenv("HOME"), CONFIG_FILE_DIRECTORY CONFIG_FILE);
   config_file = fopen(data, "r"); if(config_file == 0) return 1;
   while (fgets(data,CONFIG_LINE_CHARACTER_MAX-1, config_file) != 0) {
        if(strlen(data) < 5) continue;
        sscanf(data, "%s %i", option, &value);
        if(option[0] == '#') continue;
        if(option[0] == '[')
            if(!strcmp(option, "[config]")) {key = true; continue;} else if (key) break;
        if(key) {
            if(!strcmp(option, "synth_channel_out"     )) {synth_channel_out      = value; continue;}
            if(!strcmp(option, "wrong_note_key"        )) {wrong_note_key         = value; continue;}
            if(!strcmp(option, "wrong_note_velocity"   )) {wrong_note_velocity    = value; continue;}
            if(!strcmp(option, "guys_progress_breadth" )) {guys_progress_breadth  = value; continue;}
            if(!strcmp(option, "number_of_coming_notes")) {number_of_coming_notes = value; continue;}
            if(!strcmp(option, "tune_config_directory" )) {
                sscanf(data, "%s \"%[^\"]\"", option, tune_config_directory);
                continue;
            }
        }
   }
   fclose(config_file); return 0;
}


int read_flyid_config (fluid_settings_t* fluid_settings) {
    FILE* config_file;
    char data[CONFIG_LINE_CHARACTER_MAX];
    char option[CONFIG_OPTION_CHARACTER_MAX];
    char value[CONFIG_LINE_CHARACTER_MAX];
    bool key = false;
    tune_data[0][0] = 0;

    sprintf(data, "%s%s", getenv("HOME"), CONFIG_FILE_DIRECTORY CONFIG_FILE);
    config_file = fopen(data, "r"); if(config_file == 0) return 1;
    while (fgets(data,CONFIG_LINE_CHARACTER_MAX-1, config_file) != 0) {
        if(strlen(data) < 5) continue;
         sscanf(data, "%s %s", option, value);
        if(option[0] == '#') continue;
        if(option[0] == '[')
            if(!strcmp(option, "[fluidsynth]")) {key = true; continue;}
            else if (key) break; else continue;
        if(key) {
            if(!strcmp(option, "sound_file")){
                sscanf(data, "%s \"%[^\"]\"", option, (char*)tune_data);
                continue;
            }
            if(value[0] == '"') {
                sscanf(data, "%s \"%[^\"]\"", option, value);
                if(fluid_settings_setstr(fluid_settings, option, value)) return 1;
                continue;
            }
            if (value[strlen(value) - 1]== 'f') {
                sscanf(data, "%s %f", option, (float*)value);
                if(fluid_settings_setnum(fluid_settings, option, *((float*)value))) return 1;
            } else {
                sscanf(data, "%s %i", option, (int*)value);
                if(fluid_settings_setint(fluid_settings, option, *((int*)value))) return 1;
            }
        }
    }
    fclose(config_file); return 0;
}
