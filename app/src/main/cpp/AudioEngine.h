/*********************************************************************
 * AudioEngine.h
 * Michael Edwards - 11.18.2019
 *
 * AudioEngine creates input/recording streams and handles all audio
 * for this app.
 * This utilizes the AAudio high performance audio api.
 *********************************************************************/

#ifndef SIMPLE_TUNER_AUDIOENGINE_H
#define SIMPLE_TUNER_AUDIOENGINE_H

#include<aaudio/AAudio.h>
#include <string>
#include "Oscillator.h"
#include "AnalyzeAudio.h"

class AudioEngine {

    public:
        AudioEngine();
        ~AudioEngine();

        bool start();
        void stop();
        void restart();

        AnalyzeAudio* getAnalyzeAudio(){ return analyzeAudio; };
        Oscillator* getOscillator() { return osc;};
        aaudio_data_callback_result_t recordingCallback(float* audioData, int32_t numFrames);

    private:
        Oscillator* osc = nullptr;
        AnalyzeAudio* analyzeAudio = nullptr;
        AAudioStream* stream = nullptr;
        AAudioStream* rec_stream = nullptr;
};

#endif //SIMPLE_TUNER_AUDIOENGINE_H
