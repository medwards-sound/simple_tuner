// Created by Michael Edwards on 11/23/2019.

#include "AudioStream.h"

AudioStream::AudioStream(){
    stream = nullptr;
    AAudio_createStreamBuilder(&builder);
    AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setChannelCount(builder, 1);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
    //ADD ERROR CALLBACK
}

AudioStream::~AudioStream() {}


//initialize the stream
bool AudioStream::initStream(){

    aaudio_result_t r = AAudioStreamBuilder_openStream(builder, &stream);

    if(r == AAUDIO_OK)
        return true;
    else{

        //ADD ERROR LOGGING
        return false;
    }
}

//start the stream
bool AudioStream::startStream() {

    aaudio_result_t r = AAudioStream_requestStart(stream);

    if(r == AAUDIO_OK)
        return true;
    else{

        //ADD ERROR LOGGING
        return false;
    }
}

void AudioStream::cleanup() {

    AAudioStreamBuilder_delete(builder);
}

//getter: stream
AAudioStream* AudioStream::getStream(){

    return stream;
}