#include <stdlib.h>
#include <globals.h>

static void remote_note(int note, int channel){
    int total_number = (tune_data[tune_length][0] & 0x0f);
    int left_number = tune_data[tune_length][0] / 0x10;
    int i, b = -1, e;

    if (channel == left_hand_channel) {b = 1; e = left_number; left_number--;}
    if (channel == right_hand_channel) {b = left_number + 1; e = total_number;}
    if (b == -1) return;

    for(i = b; i <= e; i++) if (tune_data[tune_length][i] == note) break;
    for (; i < total_number; i++) tune_data[tune_length][i]=tune_data[tune_length][i+1];
    tune_data[tune_length][0] = left_number * 0x10 + total_number -1;
}

static void introduce_note(int note, int channel){
    int total_number = (tune_data[tune_length][0] & 0x0f) + 1;
    if(total_number > 10){printf("\nToo many notes.\n"); exit(1);}

    if(channel == left_hand_channel) {
        int left_number = tune_data[tune_length][0] / 0x10 + 1;
        for(int i = total_number; i > left_number; i--)
            tune_data[tune_length][i] = tune_data[tune_length][i-1];
        tune_data[tune_length][left_number] = note + 0b10000000;
        tune_data[tune_length][0] = left_number * 0x10 + total_number;
        return;
    }
    if (channel == right_hand_channel) {
        tune_data[tune_length][total_number] = note + 0b10000000;
        tune_data[tune_length][0]++;
    }
}

static void copy_tuple(){
    int i, j, total = tune_data[tune_length][0] & 0xf, count = total;

    if (tune_length > 0)
        for (i = 1; i <= total; i++)
            for (j = 1; j <= (tune_data[tune_length-1][0] & 0xf); j++)
                if (tune_data[tune_length][i] == (tune_data[tune_length -1][j] & 0b01111111)) {count--; break;}
    if(tune_data[tune_length][0] == 0 || count == 0) return;

    if(tune_length < TUNE_LENGTH_MAX - 1) tune_length++;
        else {printf("\nTune is too long.\n"); exit(1);}

    tune_data[tune_length][0]=tune_data[tune_length-1][0];
    for (i=1; i <= total; i++)
        tune_data[tune_length][i]=tune_data[tune_length-1][i] & 0b01111111;
}

static int get_midi_file_event(void* tick, fluid_midi_event_t *event){
    static int old_tick;

    if(fluid_midi_event_get_type(event) == 0x90) {
        if (tune_length == -1) {tune_length = 0; old_tick = 0;}
        if (*(int*)tick == old_tick)
            if(fluid_midi_event_get_velocity(event)==0) remote_note(fluid_midi_event_get_key(event), fluid_midi_event_get_channel(event));
            else introduce_note(fluid_midi_event_get_key(event), fluid_midi_event_get_channel(event));
        else {
            copy_tuple();
            if(fluid_midi_event_get_velocity(event)==0) remote_note(fluid_midi_event_get_key(event), fluid_midi_event_get_channel(event));
            else introduce_note(fluid_midi_event_get_key(event), fluid_midi_event_get_channel(event));
        }
        old_tick = *(int*)tick;
    }
    return FLUID_OK;
}

static int update_midi_tick(void* tick, int curr_tick){
    *(int*)tick = curr_tick;
    return FLUID_OK;
}

int read_midi_file_with_libfluidsynth(char* midi_file){
    fluid_settings_t* fluid_settings;
    fluid_synth_t* fluid_synth;
    fluid_player_t* fluid_player;
    fluid_file_renderer_t* fluid_file_renderer;
    int tick = 0;
    tune_length = -1; tune_data[0][0] = 0;

    fluid_settings = new_fluid_settings();
    fluid_settings_setstr(fluid_settings, "audio.file.name", NULL_FILE_NAME);
    fluid_settings_setstr(fluid_settings, "player.timing-source", "sample");
    fluid_settings_setint(fluid_settings, "synth.lock-memory", 0);
    fluid_synth = new_fluid_synth(fluid_settings);
    fluid_player = new_fluid_player(fluid_synth);
    fluid_player_set_playback_callback(fluid_player, get_midi_file_event,  &tick);
    fluid_player_set_tick_callback(fluid_player, update_midi_tick, &tick);
    fluid_player_add(fluid_player, midi_file);
    fluid_file_renderer = new_fluid_file_renderer (fluid_synth);
    fluid_player_play(fluid_player);
    while (fluid_player_get_status(fluid_player) == FLUID_PLAYER_PLAYING)
        if (fluid_file_renderer_process_block(fluid_file_renderer) != FLUID_OK) break;
    fluid_player_join(fluid_player);
    delete_fluid_file_renderer(fluid_file_renderer);
    delete_fluid_player(fluid_player);
    delete_fluid_synth(fluid_synth);
    delete_fluid_settings(fluid_settings);
    return tune_length < 1;
}
