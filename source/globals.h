#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <fluidsynth.h>

extern uint8_t tune_data[][11];     //[0] &0x0f - total number; /0x10 - left hand
                                    //>0x0f - NoteOn event; <0x0f - NoteOf event was not happen
                                    //left hand notes first then right hand notes
extern int tune_length;
#define TUNE_LENGTH_MAX 1000

extern int left_hand_channel;
extern int right_hand_channel;

extern fluid_synth_t* fluid_synth;

extern int synth_channel_out;
extern int wrong_note_key;
extern int wrong_note_velocity;
extern int guys_progress_breadth;
extern int number_of_coming_notes;

//Directional Buttons and Transport Controls event=0xb0 On/Off = 127/0 (velocity)
//nuber is in key field

#define MIDI_KEYBOARD_ENTER_BUTTON  20
#define MIDI_KEYBOARD_LEFT_BUTTON   21
#define MIDI_KEYBOARD_RIGHT_BUTTON  22
#define MIDI_KEYBOARD_UP_BUTTON     23
#define MIDI_KEYBOARD_DOWN_BUTTON   24

#define MIDI_KEYBOARD_PAUSE_CONTROL 118
#define MIDI_KEYBOARD_PLAY_CONTROL  119
#define MIDI_KEYBOARD_STOP_CONTROL  114

#define NULL_FILE_NAME "/dev/null"

#define CONFIG_FILE_DIRECTORY "/.config/PianoBG/"
#define CONFIG_FILE           "PianoBG.conf"

#endif // GLOBALS_H
