/***************************************************************
 * NoteMap.h
 * Michael Edwards - 11.19.2019
 *
 * NoteMap stores prerendered sine waves, both in phase and
 * 90 degrees out of phase. Each instance of NoteMap contains
 * an array for in tune, sharp, and flat versions of each of
 * these.
 *
 ***************************************************************/

#ifndef SIMPLE_TUNER_NOTEMAP_H
#define SIMPLE_TUNER_NOTEMAP_H

#include <array>
#include <string>
#include "Oscillator.h"
#include "Definitions.h"

class NoteMap {

    public:

        NoteMap();
        NoteMap(std::string name, float freq);
        ~NoteMap();

        void initMap(std::string name, float freq){
            noteName = name;
            generate(freq);
        }

        float* getNote(int type, bool phaseShift);
        std::string getName(){return noteName;};

    private:
        void generate(float freq);

        float baseNoteInPhase[SAMPLE_LIMIT];
        float lowNoteInPhase[SAMPLE_LIMIT];
        float highNoteInPhase[SAMPLE_LIMIT];

        float baseNoteOutPhase[SAMPLE_LIMIT];
        float lowNoteOutPhase[SAMPLE_LIMIT];
        float highNoteOutPhase[SAMPLE_LIMIT];

        std::string noteName = "";
};


#endif //SIMPLE_TUNER_NOTEMAP_H
