//
// Created by Michael Edwards on 11/23/2019.
//

#ifndef SIMPLE_TUNER_AUDIOSTREAM_H
#define SIMPLE_TUNER_AUDIOSTREAM_H

#include<aaudio/AAudio.h>
#include "Definitions.h"
//#include "AudioEngine.h"
#include<string>

enum StreamStatus{ WORKING, STOPPED};
enum STREAM_TYPE{ ANALYSIS, SYNTHESIS};

class AudioStream {

    public:
        AudioStream();
        ~AudioStream();
        StreamStatus getStreamStatus();
        AAudioStream* getStream();
        void setStreamStatus(StreamStatus s);
        bool initStream();
        virtual bool startStream();


    protected:
        AAudioStreamBuilder* builder;
        AAudioStream* stream;
    private:

        StreamStatus status;

        void createRecordingStream();
        void createOscillatorStream();


};


#endif //SIMPLE_TUNER_AUDIOSTREAM_H
