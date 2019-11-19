/*************************************************************************
 * AnalyzeAudio.h
 * Michael Edwards - 11.19.2019
 *
 * AnalyzeAudio will analyze a waveform in order to determine
 * what pitch is being played into microphone.
 *
 * The algorithm used to match pitch is inspired by the method
 * used by Jonathan Bergknoff seen here:
 * https://github.com/jbergknoff/guitar-tuner
 *
 * Before any processing occurs, an array of NoteMap is generated to hold
 * every note and slighly altered versions of that note.
 * (in phase, out of phase, sharp, flat)
 *
 * Once in use, an input buffer is filled from the audio engine. When full,
 * analysis begins.
 *
 * Analysis combines all the in phase and out of phase note versions into
 * separate arrays. Next the magnitude for sharp, flat, normal versions of
 * each note are calculated for every defined note. The maximum of these is
 * found, divided by the overall average and finally compared to a threshold.
 * If above the threshold, then a match is found.
 *
 * Lastly, once this is complete the buffer is refilled with the next section
 * to analyze.
 *************************************************************************/

#ifndef SIMPLE_TUNER_ANALYZEAUDIO_H
#define SIMPLE_TUNER_ANALYZEAUDIO_H

#include <cstdint>
#include <array>
#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include "Oscillator.h"
#include "Definitions.h"
#include "NoteMap.h"

const std::unordered_map<std::string, float> notes{
        {"C2", 65.41f},
        {"D2", 73.42f},
        {"E2", 82.41f},
        {"F2", 87.31f},
        {"G2", 98.0f},

        {"A2", 110.0f},
        {"B2", 123.47f},
        {"C3", 130.81f},
        {"D3", 146.83f},
        {"E3", 164.81f},

        {"F3", 174.61f},
        {"G3", 196.0f},
        {"A3", 220.0f},
        {"B3", 245.94f},
        {"C4", 261.63f},

        {"D4", 293.66f},
        {"E4", 329.63f},
        {"Db2", 69.3f},
        {"Eb2", 77.78f},
        {"Gb2", 92.5f},

        {"Ab2", 103.83f},
        {"Bb2", 115.54f},
        {"Db3", 138.59f},
        {"Eb3", 155.56f},
        {"Gb3", 185.0f},

        {"Ab3", 207.65f},
        {"Bb3", 233.08f},
        {"Db4", 277.18f},
        {"Eb4", 311.13f}
};

const int noteCt = 29;


class AnalyzeAudio {

    public:
        AnalyzeAudio();
        ~AnalyzeAudio();

        int32_t analyze(float* data, int32_t sampleCount);
        std::string getCurrNote();
        bool isBufferFull(){return buffer.size() - 1 == bufferIndex;};
        bool isProcessing(){return processing;};
        std::string getQuality();
        void setTunerOn(bool on){tunerOn = on;};
        float getNoteFreq();

    private:
        bool tunerOn;
        bool processing;
        int bufferIndex;
        std::string currNote;
        int currQuality;

        std::unordered_map<float, std::string> notesReverse;
        std::array<float, SAMPLE_LIMIT> buffer {0};

        NoteMap* noteMap;

        void findFundamental();
        void generateComparators();
        void generateReverseNotes();
};


#endif //SIMPLE_TUNER_ANALYZEAUDIO_H
