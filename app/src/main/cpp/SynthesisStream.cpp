// Created by Michael Edwards on 11/24/2019.

#include "SynthesisStream.h"

//callback to render sine wave via oscillator
aaudio_data_callback_result_t  SynthesisStream::synthCallback(AAudioStream* stream, void* userData, void* audioData, int32_t frames){

    ((Oscillator*) (userData)) -> render(static_cast<float*> (audioData), frames);

    return  AAUDIO_CALLBACK_RESULT_CONTINUE;
}

SynthesisStream::SynthesisStream() {

    oscillator = new Oscillator();

    AAudioStreamBuilder_setDataCallback(AudioStream::builder, SynthesisStream::synthCallback, oscillator);

}

SynthesisStream::~SynthesisStream() {

    delete oscillator;
}

//alter buffer size, start stream
bool SynthesisStream::startStream(){

    AAudioStream_setBufferSizeInFrames(AudioStream::stream, AAudioStream_getFramesPerBurst(AudioStream::stream) * BUFFER_BURST_SIZE);

    aaudio_result_t r = AAudioStream_requestStart(stream);

    if(r == AAUDIO_OK)
        return true;
    else{

        //AudioEngine::logEngineError(r);
        return false;
    }
}

