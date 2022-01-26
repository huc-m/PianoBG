#include <globals.h>
#include <stdio.h>

static char* note_from_midi_code(uint8_t code, char note[4]){
    static const char* name="C C#D D#E F F#G G#A A#B ";
    ((uint16_t*) note)[1] = ' ';
    ((uint16_t*) note)[0] = ((uint16_t*) name)[code % 12];
    note[1 + (note[1] == '#')] = '0' + code / 12 - 1;
    return note;
}

void print_guys_progress(int position) {
    int progress = position * guys_progress_breadth / tune_length;
    printf("\r>%0*i%*c", progress, 0, guys_progress_breadth - progress + 1, '<');
}

void print_guys_wrong_note(int position, int wrong_note_key) {
    char note[4];
    printf("\nWrong note %s on position %i.\n", note_from_midi_code(wrong_note_key, note), position);
}

void print_coming_notes(int position) {
    char note[4];
    int i, j, k, jm;
    int end = position + number_of_coming_notes; if(end > tune_length) end = tune_length;
    int8_t max, max_h, temp[10];

    printf("\n");
    for(max_h = 0, i = position; i < end; i++)
        for(j = 1; j <= tune_data[i][0] / 0x10; j++) {
            for(k = 0; k < max_h; k++) if((tune_data[i][j]&0b01111111) == temp[k]) break;
            if(k == max_h) temp[max_h++] = tune_data[i][j]&0b01111111;
        }
    for(k = 0; k < max_h; k++){
        for(max = temp[jm = k], j = k + 1; j < max_h; j++) if(temp[j] > max) max = temp[jm = j]; temp[jm] = temp[k];
        for(i = position; i < end; i++) {
            *((int32_t*)note) = *((int32_t*)"|  ");
            for(j = 1; j <= tune_data[i][0]/0x10; j++)
                if((tune_data[i][j]&0b01111111) == max) {
                    note_from_midi_code(max, note); if(tune_data[i][j] < 0b10000000) note[0] += 'a' - 'A';
                    break;
                }
            printf("%-4s", note);
        }
        printf("\n");
    }

    for(i = position; i < end; i++) printf("+---"); printf("\n");

    for(max_h = 0, i = position; i < end; i++)
        for(j = tune_data[i][0] / 0x10+1; j <= (tune_data[i][0]&0x0f); j++) {
            for(k = 0; k < max_h; k++) if((tune_data[i][j]&0b01111111) == temp[k]) break;
            if(k == max_h) temp[max_h++] = tune_data[i][j]&0b01111111;
        }
    for(k = 0; k < max_h; k++){
        for(max = temp[jm = k], j = k + 1; j < max_h; j++) if(temp[j] > max) max = temp[jm = j]; temp[jm] = temp[k];
        for(i = position; i < end; i++) {
            *((int32_t*)note) = *((int32_t*)"|  ");
            for(j = tune_data[i][0] / 0x10+1; j <= (tune_data[i][0]&0x0f); j++)
                if((tune_data[i][j]&0b01111111) == max) {
                    note_from_midi_code(max, note); if(tune_data[i][j] < 0b10000000) note[0] += 'a' - 'A';
                    break;
                }
            printf("%-4s", note);
        }
        printf("\n");
    }
}
