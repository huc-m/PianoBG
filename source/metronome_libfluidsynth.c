// The code is get from "https://www.fluidsynth.org/api/fluidsynth_metronome_8c-example.html"
// with small adjustments

/* FluidSynth Metronome - Sequencer API example
 * This code is in the public domain.
 * [Pedro Lopez-Cabanillas <plcl@users.sf.net>]
 */

#include <globals.h>

static fluid_sequencer_t *sequencer = NULL;
short int synth_destination, client_destination;
int time_marker;

int metronome_tempo = 120;
int metronome_note_duration;
int metronome_weak_note = 100;
int metronome_strong_note = 90;
int metronome_pattern_size = 4;
int metronome_velocity = 80;
int metronome_channel = 0;


static void schedule_noteon(int chan, short key, unsigned int ticks) {
    fluid_event_t *ev = new_fluid_event();
    fluid_event_set_source(ev, -1);
    fluid_event_set_dest(ev, synth_destination);
    fluid_event_noteon(ev, chan, key, metronome_velocity);
    fluid_sequencer_send_at(sequencer, ev, ticks, 1);
    delete_fluid_event(ev);
}

static void schedule_timer_event(void) {
    fluid_event_t *ev = new_fluid_event();
    fluid_event_set_source(ev, -1);
    fluid_event_set_dest(ev, client_destination);
    fluid_event_timer(ev, NULL);
    fluid_sequencer_send_at(sequencer, ev, time_marker, 1);
    delete_fluid_event(ev);
}

static void schedule_pattern(void) {
    int note_time;
    note_time = time_marker;
    for(int i = 0; i < metronome_pattern_size; ++i) {
        schedule_noteon(metronome_channel, i ? metronome_weak_note : metronome_strong_note, note_time);
        note_time += metronome_note_duration;
    }
    time_marker = note_time;
}

static void sequencer_callback(unsigned int time, fluid_event_t *event, fluid_sequencer_t *seq, void *data) {
    schedule_timer_event();
    schedule_pattern();
}

void metronome (bool delete){
    if(delete) {
        if (!sequencer) delete_fluid_sequencer(sequencer);
        return;
    }
    if(sequencer) {
        delete_fluid_sequencer(sequencer);
        sequencer = NULL;
    } else {
        metronome_note_duration = 60000 / metronome_tempo;
        sequencer = new_fluid_sequencer2(0);
        synth_destination = fluid_sequencer_register_fluidsynth(sequencer, fluid_synth);
        client_destination = fluid_sequencer_register_client(sequencer, "fluidsynth_metronome", sequencer_callback, NULL);
        time_marker = fluid_sequencer_get_tick(sequencer);
        schedule_pattern();
        schedule_timer_event();
        schedule_pattern();
    }
}
