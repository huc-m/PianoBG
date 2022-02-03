#include <globals.h>
#include <stdio.h>

char* note_from_midi_code(uint8_t note_key, char note[4]){
    static const char* name="C C#D D#E F F#G G#A A#B ";
    ((uint16_t*) note)[1] = ' ';
    ((uint16_t*) note)[0] = ((uint16_t*) name)[note_key % 12];
    note[1 + (note[1] == '#')] = '0' + note_key / 12 - 1;
    return note;
}

static bool note_from_note_key(uint8_t note_key, char note[2]){ // true => C#; false => C
    static const char* name="C C#D D#E F F#G G#A A#B c c#d d#e f f#g g#a a#b ";
    ((uint16_t*)note)[0] = note_key > 0b10000000 ?
        ((uint16_t*)name)[(note_key & 0b01111111) % 12] :
        ((uint16_t*)&name[24])[(note_key & 0b01111111) % 12];
    return note[1] == '#';
}

int r(int note_key){
    int temp = note_key % 12;
    return (temp <= 4 ? temp : temp + 1) / 2 + note_key / 12 * 7;
}

void print_guys_progress(int position) {
    int progress = position * guys_progress_breadth / tune_length;
    printf("\r>%0*i%*c", progress, 0, guys_progress_breadth - progress + 1, '<');
}

void print_guys_wrong_note(int position, int wrong_note_key) {
    char note[4];
    printf("\nWrong note %s on position %i (total %i).\n", note_from_midi_code(wrong_note_key, note), position, ++number_of_wrong_plays);
}

int print_coming_notes_gs(int position) { // r_hand staff E4(64) - F5(77)
                                        // l_hand staff G2(43) - A3(57)
                                        // CC#B C#CB C__B  C#_B
    int hight_pos = 77, low_pos = 43;
    int end = position + number_of_coming_notes; if(end > tune_length) end = tune_length;
    char note[4], temp_note[2];
    int h_pos, v_pos, i;
    int8_t temp;
    bool note_type, note_exists;

    for(h_pos = position; h_pos < end; ++h_pos)
        for(v_pos = 1; v_pos <= (tune_data[h_pos][0]&0x0f); ++v_pos) {
            if((temp = tune_data[h_pos][v_pos]&0b01111111) > hight_pos) hight_pos = temp;
            if(temp < low_pos) low_pos = temp;
        }
    printf("\n");
    for (v_pos = r(hight_pos); v_pos >= r(low_pos); --v_pos) {
        for(h_pos = position; h_pos < end; ++h_pos) {
            note_exists = false; *(int32_t*)note = v_pos % 2 && ((v_pos >= r(43) && v_pos <= r(57)) || (v_pos >= r(64) && v_pos <= r(77))) ?
                *(int32_t*)"---" : h_pos % 2 ? *(int32_t*)"   " : *(int32_t*)".  ";
            for(i = 1; i <= (tune_data[h_pos][0]&0x0f); ++i)
                if ((r(tune_data[h_pos][i]&0b01111111)) == v_pos){
                    if(note_exists)
                        if (note_type == note_from_note_key(tune_data[h_pos][i], temp_note)) continue;
                        else {
                            if(note_type) note[2] = temp_note[0]; else *(int16_t*)&note[1] = *(int16_t*)temp_note;
                            break;
                        }
                    else
                        note_exists = true; note_type = note_from_note_key(tune_data[h_pos][i], note);
                    }
            printf("%.3s", note);
        }
        printf("\n");
    }
    return r(hight_pos) - r(low_pos);
}

int print_coming_notes(int position) {
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
    for(k = 0; k < max_h; k++) {
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
    int stature = max_h;

    for(max_h = 0, i = position; i < end; i++)
        for(j = tune_data[i][0] / 0x10+1; j <= (tune_data[i][0]&0x0f); j++) {
            for(k = 0; k < max_h; k++) if((tune_data[i][j]&0b01111111) == temp[k]) break;
            if(k == max_h) temp[max_h++] = tune_data[i][j]&0b01111111;
        }
    for(k = 0; k < max_h; k++) {
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
    return stature + max_h;
}

