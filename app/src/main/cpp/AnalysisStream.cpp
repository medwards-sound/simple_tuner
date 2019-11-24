//
// Created by Michael Edwards on 11/24/2019.
//

#include "AnalysisStream.h"

aaudio_data_callback_result_t  AnalysisStream::analysisCallback(AAudioStream* stream, void* userData, void* audioData, int32_t frames){

    return ((AnalysisStream*)(userData))->analysis(static_cast<float*>(audioData), frames);
}

aaudio_data_callback_result_t  AnalysisStream::analysis(float* audioData, int32_t frames){

    if(!analyzeAudio->isProcessing()){

        analyzeAudio->analyze(audioData, frames);
    }

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

AnalysisStream::AnalysisStream() {

    analyzeAudio = new AnalyzeAudio();

    AAudioStreamBuilder_setDirection(AudioStream::builder, AAUDIO_DIRECTION_INPUT);
    AAudioStreamBuilder_setSampleRate(AudioStream::builder, SAMPLE_RATE);
    AAudioStreamBuilder_setDataCallback(AudioStream::builder, AnalysisStream::analysisCallback, this);

}

AnalysisStream::~AnalysisStream() {

}








