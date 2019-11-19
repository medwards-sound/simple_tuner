/**************************************************************
 * AudioEngine.cpp
 * Michael Edwards - 09.18.2019
 *
 * See header file for details.
 *
 **************************************************************/

#include "AudioEngine.h"
#include <android/log.h>
#include <thread>
#include <mutex>

constexpr int32_t kBufferSizeInBursts = 4;

AudioEngine::AudioEngine(){}
AudioEngine::~AudioEngine() {
    if(osc != nullptr)
        delete osc;
    if(analyzeAudio != nullptr)
        delete analyzeAudio;
}

aaudio_data_callback_result_t dataCallback
        (AAudioStream* stream, void* userData, void* audioData, int32_t frames){

    ((Oscillator*) (userData)) -> render(static_cast<float*> (audioData), frames);

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

aaudio_data_callback_result_t recordingDataCallback(
        AAudioStream* stream,
        void* userData,
        void* audioData,
        int32_t numFrames){

    return ((AudioEngine *) (userData))->recordingCallback(static_cast<float *>(audioData), numFrames);
}

aaudio_data_callback_result_t AudioEngine::recordingCallback(float* audioData, int32_t numFrames){

    if(!analyzeAudio->isProcessing()){

        analyzeAudio->analyze(audioData, numFrames);
    }

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void errorCallback(AAudioStream* stream, void* userData, aaudio_result_t error){

    if(error == AAUDIO_ERROR_DISCONNECTED){

        std::function<void(void)> restartFunction =
                std::bind(&AudioEngine::restart, static_cast<AudioEngine*> (userData));

        new std::thread(restartFunction);
    }
}

//initialize record and playback stream
bool AudioEngine::start() {

    analyzeAudio = new AnalyzeAudio(); //for audio analysis (tuner)
    osc = new Oscillator();

    //oscillator stream
    AAudioStreamBuilder* streamBuilder;
    AAudio_createStreamBuilder(&streamBuilder);
    AAudioStreamBuilder_setFormat(streamBuilder, AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setChannelCount(streamBuilder, 1);
    AAudioStreamBuilder_setPerformanceMode(streamBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setSharingMode(streamBuilder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setDataCallback(streamBuilder, ::dataCallback, osc);
    AAudioStreamBuilder_setErrorCallback(streamBuilder, ::errorCallback, this);

    //record stream
    AAudioStreamBuilder* recordingBuilder;
    AAudio_createStreamBuilder(&recordingBuilder);
    AAudioStreamBuilder_setDirection(recordingBuilder, AAUDIO_DIRECTION_INPUT);
    AAudioStreamBuilder_setPerformanceMode(recordingBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setSharingMode(recordingBuilder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setFormat(recordingBuilder,AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setChannelCount(recordingBuilder, 1);
    AAudioStreamBuilder_setSampleRate(recordingBuilder, SAMPLE_RATE);
    AAudioStreamBuilder_setDataCallback(recordingBuilder, ::recordingDataCallback, this);
    AAudioStreamBuilder_setErrorCallback(recordingBuilder, ::errorCallback, this);

    //init streams
    aaudio_result_t result_init = AAudioStreamBuilder_openStream(streamBuilder, &stream);
    aaudio_result_t rec_result_init = AAudioStreamBuilder_openStream(recordingBuilder, &rec_stream);

    if(result_init != AAUDIO_OK){

        __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "Error init stream %s", AAudio_convertResultToText(result_init));

        return false;
    }

    if(rec_result_init != AAUDIO_OK){

        __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "Error init stream RECORDING %s", AAudio_convertResultToText(rec_result_init));

        return false;
    }

    //set buffer size
    AAudioStream_setBufferSizeInFrames(stream, AAudioStream_getFramesPerBurst(stream) * kBufferSizeInBursts);

    //start stream
    aaudio_result_t  result_start = AAudioStream_requestStart(stream);
    aaudio_result_t  rec_result_start = AAudioStream_requestStart(rec_stream);

    if(result_start != AAUDIO_OK){

        __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "Error starting stream %s", AAudio_convertResultToText(result_start));

        return false;
    }

    if(rec_result_start != AAUDIO_OK){

        __android_log_print(ANDROID_LOG_ERROR, "AudioEngine", "Error starting stream %s", AAudio_convertResultToText(rec_result_start));

        return false;
    }

    AAudioStreamBuilder_delete(streamBuilder);
    AAudioStreamBuilder_delete(recordingBuilder);

    return true;
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

    if(stream != nullptr){

        AAudioStream_requestStop(stream);
        AAudioStream_close(stream);
    }

  if(rec_stream != nullptr){

        AAudioStream_requestStop(rec_stream);
        AAudioStream_close(rec_stream);
    }
}

//set audio engine to play audio from oscillator
void AudioEngine::toneOn(bool on) {

    osc->waveOn(on);
}




