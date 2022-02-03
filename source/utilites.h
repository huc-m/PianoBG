#ifndef UTILITES_H
#define UTILITES_H

int print_coming_notes(int position);
int print_coming_notes_gs(int position);
void print_guys_progress(int position);
void print_guys_wrong_note(int position, int wrong_note_key);
char* note_from_midi_code(uint8_t note_key, char note[4]);

#endif // UTILITES_H
