#include <stdbool.h>
#include <globals.h>
#include <midi_with_libfluidsynth.h>
#include <config.h>

uint8_t tune_data[TUNE_LENGTH_MAX][11];
fluid_synth_t* fluid_synth;

int tune_length;

int main(int argvc, char* argv[]){
    char *tune_config_directory = (char*)&tune_data[50][0];
    if(read_config(tune_config_directory)) {printf("\nBad congiguration file.\n"); return 1;}
    fluid_settings_t* fluid_settings;
    fluid_settings = new_fluid_settings();
    if(read_flyid_config(fluid_settings)) {
        delete_fluid_settings(fluid_settings);
        printf("\nBad fluid configuration.\n");
        return 1;
    }
    fluid_synth = new_fluid_synth(fluid_settings);
    if(fluid_synth_sfload(fluid_synth, (char*)tune_data, true) == FLUID_FAILED) {
            delete_fluid_synth(fluid_synth);
            delete_fluid_settings(fluid_settings);
            printf("\nBad sound font.\n");
            return 1;
     }
    if(read_tune_config(argv[1], tune_config_directory)) {
            delete_fluid_synth(fluid_synth);
            delete_fluid_settings(fluid_settings);
            printf("\nBad tune configuration.\n");
            return 1;
     }
    if (read_midi_file_with_libfluidsynth(tune_config_directory)) {
        printf("\nFile not foind.\n"); return 1;
    }
    fluid_audio_driver_t* fluid_audio_driver = new_fluid_audio_driver(fluid_settings, fluid_synth);
    read_midi_keyboard_with_libfluidsynth(fluid_settings);

    delete_fluid_audio_driver(fluid_audio_driver);
    delete_fluid_synth(fluid_synth);
    delete_fluid_settings(fluid_settings);
    return 0;
}


