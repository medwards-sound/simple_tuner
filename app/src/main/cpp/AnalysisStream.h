//
// Created by Michael Edwards on 11/24/2019.
//

#ifndef SIMPLE_TUNER_ANALYSISSTREAM_H
#define SIMPLE_TUNER_ANALYSISSTREAM_H

#include "AudioStream.h"
#include "AnalyzeAudio.h"

class AnalysisStream : public AudioStream {

    public:
        AnalysisStream();
        ~AnalysisStream();

        AnalyzeAudio* getAnalyzeAudio(){return analyzeAudio;};

        aaudio_data_callback_result_t analysis(float* audioData, int32_t frames);
        static aaudio_data_callback_result_t analysisCallback(AAudioStream* stream, void* userData, void* audioData, int32_t frames);

    private:
        AnalyzeAudio* analyzeAudio;


};


#endif //SIMPLE_TUNER_ANALYSISSTREAM_H
