//AudioEngine.cpp, Michael Edwards, 11.19.2019

#include "AudioEngine.h"
#include <android/log.h>
#include <thread>
#include <mutex>


AudioEngine::AudioEngine(){


}
AudioEngine::~AudioEngine() {

    if(synthStream != nullptr)
        delete synthStream;

    if(analysisStream != nullptr)
        delete analysisStream;
}


void AudioEngine::errorCallback(AAudioStream* stream, void* userData, aaudio_result_t error){

    if(error == AAUDIO_ERROR_DISCONNECTED){

        std::function<void(void)> restartFunction =
                std::bind(&AudioEngine::restart, static_cast<AudioEngine*> (userData));

        new std::thread(restartFunction);
    }
}

//initialize record and playback stream
bool AudioEngine::start() {

    synthStream = new SynthesisStream();
    analysisStream = new AnalysisStream();

    //init stream or fail to start engine
    if(!synthStream->initStream())
        return false;
    if(!analysisStream->initStream())
        return false;

    //start stream or fail to start engine
    if(!synthStream->startStream())
        return false;
    if(!analysisStream->startStream())
        return false;

    synthStream->cleanup();
    analysisStream->cleanup();

    return true; //engine started
}

void AudioEngine::logEngineError(aaudio_result_t r){
    __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "Error starting stream %s", AAudio_convertResultToText(r));
}

void AudioEngine::restart() {

    static std::mutex restartingLock;

    if(restartingLock.try_lock()){

        stop();
        start();
        restartingLock.unlock();
    }
}

void AudioEngine::stop() {

    if(synthStream != nullptr){

        AAudioStream_requestStop(synthStream->getStream());
        AAudioStream_close(synthStream->getStream());
        delete synthStream;
        synthStream = nullptr;
    }

  if(analysisStream != nullptr){

        AAudioStream_requestStop(analysisStream->getStream());
        AAudioStream_close(analysisStream->getStream());
        delete analysisStream;
        analysisStream = nullptr;
    }


}




