//Oscillator.cpp, Michael Edwards, 11.19.2019

#include "Oscillator.h"
#include <math.h>

//change frequency
void Oscillator::alterFreq(float newFreq){

    freq = newFreq;
    increment = (TWO_PI * freq) / (double) SAMPLE_RATE;
}

void Oscillator::oscillatorOn(bool on){

    playing.store(on);
}

void Oscillator::fadeOut() {

    //add in future
}

//render sine wave for streaming playback
void Oscillator::render(float* data, int32_t frames){

    if(!playing.load())
        phase = 0.0;

    for(int i = 0; i < frames; i++){

        if(playing.load()){

            //next sample value
            data[i] = (float) sin(phase) * amp;

            //increment phase
            phase += increment;

            //wraparound
            if(phase > TWO_PI)
                phase -= TWO_PI;
        }

        else
            data[i] = 0;
    }
}

//create and store sine wave in array
void Oscillator::preRender(float* data, float freq, bool phaseShift){

    double localPhase = 0.0;
    double localIncrement = (TWO_PI * freq) / (double) SAMPLE_RATE;

    for(int i = 0; i < SAMPLE_LIMIT; i++){

        //phase shift uses cos to create 90 degree out of phase signal
        if(phaseShift)
            data[i] = (float) cos(localPhase) * .3;
        else
            data[i] = (float) sin(localPhase) * .3;

        //increment position
        localPhase += localIncrement;

        //wraparound
        if(localPhase > TWO_PI)
            localPhase -= TWO_PI;
    }
}