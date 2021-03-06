TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        config.c \
        main.c \
        metronome_libfluidsynth.c \
        midi_file_libfluidsynth.c \
        midi_keyboard_libfluidsynth.c \
        tune_config.c \
        utilites.c

HEADERS += \
    config.h \
    globals.h \
    midi_with_libfluidsynth.h \
    utilites.h

LIBS += \
    -lfluidsynth

DISTFILES += \
    ../binary/PianoBG \
    ../binary/libfluidsynth.so.3.0.4 \
    ../config/PianoBG.conf \
    ../config/tune.conf \
    ../utilites/play.sh
