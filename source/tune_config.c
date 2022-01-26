#include <string.h>
#include <stdio.h>
#include <globals.h>


int left_hand_channel = 0;
int right_hand_channel = 1;

int read_tune_config (char* config_file, char* midi_file){
    FILE* conf_file;
    char data[CONFIG_LINE_CHARACTER_MAX];
    char option[CONFIG_OPTION_CHARACTER_MAX];
    int value;

    conf_file = fopen(strcat(midi_file, config_file),"r");  if(conf_file == 0) return 1;
    while (fgets(data,CONFIG_LINE_CHARACTER_MAX-1, conf_file) != 0) {
         if(strlen(data) < 5) continue;
         sscanf(data, "%s %i", option, &value);
         if(option[0] == '#') continue;

         if(!strcmp(option, "left_hand_channel" )) {left_hand_channel  = value; continue;}
         if(!strcmp(option, "right_hand_channel")) {right_hand_channel = value; continue;}
         if(!strcmp(option, "midi_file_full_path" )) {
            sscanf(data, "%s \"%[^\"]\"", option, midi_file);
            continue;
         }
    }
    fclose(conf_file); return 0;
}
