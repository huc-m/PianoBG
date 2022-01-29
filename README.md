# PianoBG
MIDI keyboard piano for piano beginner.

#### Introduction
PianoBG (BeGinner) is aspired from PianoBooster. PianoBoster is great program, very elaborated and rich of functionality. However in some cases that creates problems for me as a (sheer) beginner pianist. Thus make me to write this simple program that does (I hope) only one thing – check if the guy presses the proper keys in the due order.

#### Functionality
This is the initial version that does almost nothing but check if the pianist presses only right keys in a succession specified in a MIDI file.

There are utilized the follows control keys (transport controls mist be switched to the MIDI protocol):
* "Play" – start again;
* "Pause" – print some number of incoming notes (does not help);
* "Stop" – quit;
* "Left arrow" – go to begining of incoming notes.

#### Usage
There are two configuration files:
* main configuration "PianoBG.conf" which must be in "~.config/PianoBG/";
* each tune has its own configuration which is placed in the directory mentioned in the "PianoBG.conf".

The program is started as:
```
PianoPG tune.conf
```
or
```
PianoBG 
```
for free play.

The program requires libfluidsynth, version > 2.0.

#### Compile
To compile PianoBG one must have a developer version of libfluidsynth (https://www.fluidsynth.org/) or at least its include files, version > 2.0.

To the horror of the genuine Linux devotee, there is provided a binary for Debian 11 "Bullseye" (may be contaminated with the Virus – don't try this if You do not get a shot). There is also a binary of libfluidsynth, compiled from "https://github.com/FluidSynth/fluidsynth" ("Bullseye" has an older version which is not useable).
