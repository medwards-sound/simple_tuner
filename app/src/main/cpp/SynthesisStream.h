//
// Created by Michael Edwards on 11/24/2019.
//

#ifndef SIMPLE_TUNER_SYNTHESISSTREAM_H
#define SIMPLE_TUNER_SYNTHESISSTREAM_H


#include <aaudio/AAudio.h>
#include "Oscillator.h"
#include "AudioStream.h"

class SynthesisStream : public AudioStream{

    public:
        SynthesisStream();
        ~SynthesisStream();

        bool startStream() override;

        Oscillator* getOscillator(){ return oscillator;};
        static aaudio_data_callback_result_t synthCallback(AAudioStream* stream, void* userData, void* audioData, int32_t frames);


    private:
        Oscillator* oscillator;


};


#endif //SIMPLE_TUNER_SYNTHESISSTREAM_H
