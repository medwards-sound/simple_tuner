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
#include "AudioStream.h"
#include "SynthesisStream.h"
#include "AnalysisStream.h"

class AudioEngine {
    public:
        AudioEngine();
        ~AudioEngine();
        bool start();
        void stop();
        void restart();
        static void errorCallback(AAudioStream* stream, void* userData, aaudio_result_t error);
        static void logEngineError(aaudio_result_t r);
        SynthesisStream* getSynthStream(){return synthStream;};
        AnalysisStream* getAnalysisStream(){return analysisStream;};

    private:
        SynthesisStream* synthStream = nullptr;
        AnalysisStream* analysisStream = nullptr;
};

#endif //SIMPLE_TUNER_AUDIOENGINE_H
