#include <globals.h>
#include <utilites.h>
#include <unistd.h>

int control_pipe[2];
int cur_position;
int8_t guys_tuple[11]; // [0] - total number

static void guys_tuple_furnish_note(int note_key) {
    guys_tuple[++guys_tuple[0]] = note_key;
}

static void play_guys_note(int note_key, int note_velocity) {
    for(int i = 1; i <= (tune_data[cur_position][0]&0xf); i++)
        if(note_key == (tune_data[cur_position][i]&0b01111111)) {
            fluid_synth_noteon(fluid_synth, synth_channel_out, note_key, note_velocity);
            return;
        }
    fluid_synth_noteon(fluid_synth, synth_channel_out, wrong_note_key, wrong_note_velocity);
    print_guys_wrong_note(cur_position, note_key); fflush(stdout);
}

static void guys_tuple_dispatch_note(int note_key) {
    int i;
    fluid_synth_noteoff(fluid_synth, synth_channel_out, note_key);
    for(i = 1; i <= guys_tuple[0]; i++)
        if(note_key == guys_tuple[i]) {
            for(; i < guys_tuple[0]; i++) guys_tuple[i] = guys_tuple[i + 1];
            guys_tuple[0]--;
            return;
        }
}

static void check_guys_tuple_and_go(){
    int i, j;
    int total = tune_data[cur_position][0]&0xf;

    for(i = 1; i <= guys_tuple[0]; i++) {
        for (j = 1; j <= total; j++)
            if(guys_tuple[i] == (tune_data[cur_position][j]&0b01111111)) break;
        if(j > total) return;
    }
    for(j = 1; j <= total; j++) {
        for(i = 1; i <= guys_tuple[0]; i++)
            if(guys_tuple[i] == (tune_data[cur_position][j]&0b01111111) || tune_data[cur_position][j] < 0b10000000) break;
        if(i > guys_tuple[0]) return;
    }
    guys_tuple[0] = 0;
    print_guys_progress(++cur_position); fflush(stdout);
    if(cur_position >= tune_length) write(control_pipe[1], "e", 1);
    return;
}

static int read_keyboard(void *data, fluid_midi_event_t *event){
    int event_type = fluid_midi_event_get_type(event);
    int note_key = fluid_midi_event_get_key(event);
    int note_velocity = fluid_midi_event_get_velocity(event);

    if(event_type == 0x80) {event_type = 0x90; note_velocity = 0;}
    if(event_type == 0x90) {
        if (note_velocity == 0) guys_tuple_dispatch_note(note_key);
        else {
            play_guys_note(note_key, note_velocity);
            guys_tuple_furnish_note(fluid_midi_event_get_key(event));
            check_guys_tuple_and_go();
        }
        return FLUID_OK;
    }
    if(event_type == 0xb0 && note_velocity > 0){
        switch (note_key) {
            case MIDI_KEYBOARD_STOP_CONTROL : write(control_pipe[1], "s", 1)  ; break;
            case MIDI_KEYBOARD_PLAY_CONTROL : write(control_pipe[1], "r", 1)  ; break;
            case MIDI_KEYBOARD_PAUSE_CONTROL: print_coming_notes(cur_position); fflush(stdout); break;
        }
    }
    return FLUID_OK;
}

void read_midi_keyboard_with_libfluidsynth(fluid_settings_t* fluid_settings){
    pipe(control_pipe);
    char key;
    fluid_midi_driver_t* read_keyboard_driver;
    void* keyboard_data;

    read_keyboard_driver = new_fluid_midi_driver(fluid_settings, read_keyboard, keyboard_data);

    do {
        guys_tuple[0] = 0;
        cur_position = 0;
        read(control_pipe[0], &key, 1);
        switch (key) {
            case 'e': printf("\n\n<<<<<<<<<< Tune is over. >>>>>>>>>>");
            case 'r': printf("\n\n<<<<<<<<<< Start again.  >>>>>>>>>>\n\n"); fflush(stdout);
        }
    } while (key != 's');

    delete_fluid_midi_driver(read_keyboard_driver);
    close(control_pipe[0]); close(control_pipe[1]);
}
