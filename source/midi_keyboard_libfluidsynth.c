#include <globals.h>
#include <utilites.h>
#include <midi_with_libfluidsynth.h>
#include <unistd.h>

int control_pipe[2];
int cur_position;
int8_t guys_tuple[11]; // [0] - total number

int number_of_wrong_plays;

bool notes;
int stature, notes_pos;
bool staff_view = false;

static void guys_tuple_furnish_note(int note_key) {
    guys_tuple[++guys_tuple[0]] = note_key;
}

static void play_guys_note(int note_key, int note_velocity) {
    for(int i = 1; i <= (tune_data[cur_position][0]&0xf); i++)
        if(note_key == (tune_data[cur_position][i]&0b01111111)) {
            fluid_synth_noteon(fluid_synth, synth_channel_out, note_key, note_velocity);
            return;
        }
    if(cur_position > 1)
        for(int i = 1; i <= (tune_data[cur_position - 1][0]&0xf); i++)
            if(note_key == (tune_data[cur_position - 1][i]&0b01111111)) {
                fluid_synth_noteon(fluid_synth, synth_channel_out, note_key, note_velocity);
                print_guys_wrong_note(cur_position, note_key); return;
            }
    fluid_synth_noteon(fluid_synth, synth_channel_out, wrong_note_key, wrong_note_velocity);
    print_guys_wrong_note(cur_position, note_key);
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
    guys_tuple[0] = 0; ++cur_position;
    if(notes)
        if(staff_view)
            if(cur_position >= notes_pos + number_of_coming_notes - pad_coming_notes) {
                notes_pos = cur_position - pad_coming_notes; if(notes_pos < 0) notes_pos = 0;
                printf("\e[2J\e[1H");
                stature = print_coming_notes_gs(notes_pos);
                printf("\e[%i;r\e[?6l\e[%1$iH\e[s", stature+4);
                printf("\e[s\e[%i;1H%0*i\e[u",  stature + 3, (cur_position - notes_pos) * 3, 0); fflush(stdout);
            } else {
                printf("\e[s\e[%i;1H%0*i\e[u",  stature + 3, (cur_position - notes_pos) * 3, 0);
                fflush(stdout);
            }
        else
            if(cur_position >= notes_pos + number_of_coming_notes || cur_position >= tune_length)
                {notes = false; printf("\e[;r\e[2J"); fflush(stdout);}
            else {
                printf("\e[s\e[%i;1H%0*i\e[u",  stature + 3, (cur_position - notes_pos) * 4, 0);
                fflush(stdout);
            }
    else {print_guys_progress(cur_position); fflush(stdout);}
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
            case MIDI_KEYBOARD_PAUSE_CONTROL:
                if(notes) {
                    notes = false; printf("\e[;r\e[u\e[2J"); fflush(stdout);
                } else {
                    notes = true; notes_pos = cur_position;
                    printf("\e[2J\e[1H");
                    stature = staff_view ? print_coming_notes_gs(cur_position) : print_coming_notes(cur_position);
                    printf("\e[%i;r\e[?6l\e[%1$iH\e[s", stature+4);
                }
            break;
            case MIDI_KEYBOARD_LEFT_BUTTON:
            case MIDI_KEYBOARD_RIGHT_BUTTON:
                if(notes) {
                    if(note_key == MIDI_KEYBOARD_LEFT_BUTTON)
                        {if(notes_pos > 0) notes_pos--;}
                    else if(++notes_pos >= tune_length - 1) notes_pos = tune_length - 1;
                    printf("\e[2J\e[1H");
                    stature = staff_view ? print_coming_notes_gs(cur_position = notes_pos) :  print_coming_notes(cur_position = notes_pos);
                    printf("\e[%i;r\e[?6l\e[%1$iH\e[s", stature+4);
                }
            break;
            case MIDI_KEYBOARD_ENTER_BUTTON:
                if(notes){
                    cur_position = notes_pos;  printf("\e[s\e[%i;1H\e[2K\e[u", stature + 3); fflush(stdout);
                }
            break;
            case MIDI_KEYBOARD_DOWN_BUTTON: metronome(false); break;
        }
    }
    return FLUID_OK;
}

void read_midi_keyboard_with_libfluidsynth(fluid_settings_t* fluid_settings, bool has_tune){
    pipe(control_pipe);
    char key;
    fluid_midi_driver_t* read_keyboard_driver;

    if (has_tune)
        read_keyboard_driver = new_fluid_midi_driver(fluid_settings, read_keyboard, NULL);
    else
        read_keyboard_driver = new_fluid_midi_driver(fluid_settings, fluid_synth_handle_midi_event, fluid_synth);

    do {
        notes = false; number_of_wrong_plays = 0; printf("\e[;r\e[2J");
        guys_tuple[0] = 0;
        cur_position = 0;
        read(control_pipe[0], &key, 1);
        switch (key) {
            case 'e': printf("\n\n<<<<<<<<<< Tune is over >>>>>>>>>>");
            case 'r': printf("\n\n<<<<<<<<<< Start again  >>>>>>>>>>\n\n"); fflush(stdout);
        }
    } while (key != 's');

    printf("\e[;r\e[2J");
    delete_fluid_midi_driver(read_keyboard_driver);
    metronome(true);
    close(control_pipe[0]); close(control_pipe[1]);
}
