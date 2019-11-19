/**********************************************************************
 * Oscillator.h
 *
 * Oscillator will generate sine waves. This is used for both playback
 * as well as creating prerendered arrays.
 **********************************************************************/

#ifndef SIMPLE_TUNER_OSCILLATOR_H
#define SIMPLE_TUNER_OSCILLATOR_H

#include <atomic>
#include <stdint.h>
#include "Definitions.h"
#include <array>

class Oscillator {

    public:
        Oscillator(){
            amp = 1.0;
            freq = 440.0;
            phase = 0.0;
            phaseStep = 0.0;
            alterSampleRate();
        }

        void waveOn(bool on);
        void alterSampleRate();
        void render(float* data, int32_t frames);
        void alterFreq(float newFreq);
        void fadeOut();
        static void preRender(float* data, float freq, bool phaseShift);

    private:
        std::atomic<bool> waveOn_ {false};
        double phase;
        double phaseStep;
        float amp;
        float freq;
};


#endif //SIMPLE_TUNER_OSCILLATOR_H
