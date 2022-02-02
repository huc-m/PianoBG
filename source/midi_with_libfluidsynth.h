#ifndef MIDI_WITH_LIBFLUIDSYNTH_H
#define MIDI_WITH_LIBFLUIDSYNTH_H

int read_midi_file_with_libfluidsynth(char* midi_file);
void read_midi_keyboard_with_libfluidsynth(fluid_settings_t* fluid_settings, bool has_tune);
void metronome(bool delete);

#endif // MIDI_WITH_LIBFLUIDSYNTH_H
