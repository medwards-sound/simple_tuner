//NoteMap.cpp, Michael Edwards, 11.19.2019

#include "NoteMap.h"
#include <math.h>

NoteMap::NoteMap() {}

NoteMap::NoteMap(std::string name, float freq) {

    initMap(name,freq);
}

NoteMap::~NoteMap() {}


float* NoteMap::getNote(int type, bool phaseShift) {

    if(type == 0)
        return (phaseShift ? lowNoteOutPhase : lowNoteInPhase);
    else if(type == 1)
        return (phaseShift ? baseNoteOutPhase : baseNoteInPhase);
    else if(type == 2)
        return (phaseShift ? highNoteOutPhase: highNoteInPhase);

    return NULL;
}

//generates in phase and out of phase (90 degrees) sine waves
//there is a sharp, flat, and in tune version of each
void NoteMap::generate(float freq) {

    float high = freq * powf(2.0,1.0/48.0);
   float low = freq * pow(2.0,-1.0/48.0);

   //get prerendered array from oscillator
    Oscillator::preRender(baseNoteOutPhase, freq, true);
    Oscillator::preRender(baseNoteInPhase, freq, false);
    Oscillator::preRender(lowNoteOutPhase, low, true);
    Oscillator::preRender(lowNoteInPhase, low, false);
    Oscillator::preRender(highNoteOutPhase, high, true);
    Oscillator::preRender(highNoteInPhase, high, false);
}

