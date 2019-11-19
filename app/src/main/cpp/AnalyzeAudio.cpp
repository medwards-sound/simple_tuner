//AnalyzeAudio.cpp, Michael Edwards, 11.19.2019

#include "AnalyzeAudio.h"
#include <android/log.h>
#include <math.h>

AnalyzeAudio::AnalyzeAudio() {
    tunerOn = false;
    processing = false;
    bufferIndex = 0;
    currNote = "--";
    currQuality = 1;
    generateComparators();
    generateReverseNotes();
}

AnalyzeAudio::~AnalyzeAudio() {

    delete[] noteMap;

}

void AnalyzeAudio::generateReverseNotes() {

    for(auto n : notes){

        notesReverse[n.second] = n.first;
    }
}

float AnalyzeAudio::getNoteFreq() {

    if(currNote != "--")
        return notes.at(currNote);
    else
        return 0.0;
}

std::string AnalyzeAudio::getCurrNote(){

    return currNote;
}

//return whether note is in tune, sharp, flat
std::string AnalyzeAudio::getQuality(){

    switch(currQuality){

        case 0: return "FLAT";

        case 2: return "SHARP";

        default: return " ";
    }
}

//create all waveforms to compare to buffer
void AnalyzeAudio::generateComparators(){

    noteMap = new NoteMap[notes.size()];
    int ct = 0;
    for(auto n : notes){

        NoteMap* nm = new NoteMap(n.first,n.second);
        noteMap[ct] = *nm;
        ct++;
    }
}

//fill buffer from microphone, then find fundamental note
int32_t AnalyzeAudio::analyze(float* data, int32_t sampleCount){

    if(processing || !tunerOn) {
        bufferIndex = 0;
        return sampleCount;
    }

    for(int i = 0; i < sampleCount; i++) {

        if(!isBufferFull())
            buffer[bufferIndex++] = data[i];
        else
            break;
        }

    if(isBufferFull()){
        processing = true;
        findFundamental();
        bufferIndex = 0;
    }

    return sampleCount;
}

//find best note match when compared to input buffer
void AnalyzeAudio::findFundamental() {

    //[string][0 - low out, 1 - low in, 2 - norm out, 3 - norm in, 4 - flat out, 5 - flat in]
    float stringsInPhase[noteCt][3] = {0};
    float stringsOutPhase[noteCt][3] = {0};
    float magnitudes[noteCt][3] = {0};

    float max = -1;
    int maxLoc = -1;
    float avg = 0;
    int maxQuality = 1;

    for (int i = 0; i < SAMPLE_LIMIT; i++) {

        for(int j = 0; j < noteCt; j++)
        {
            stringsOutPhase[j][0] += buffer[i] * noteMap[j].getNote(0,true)[i];
            stringsOutPhase[j][1] += buffer[i] * noteMap[j].getNote(1,true)[i];
            stringsOutPhase[j][2] += buffer[i] * noteMap[j].getNote(2,true)[i];

            stringsInPhase[j][0] += buffer[i] * noteMap[j].getNote(0,false)[i];
            stringsInPhase[j][1] += buffer[i] * noteMap[j].getNote(1,false)[i];
            stringsInPhase[j][2] += buffer[i] * noteMap[j].getNote(2,false)[i];
        }
    }

    for(int m = 0; m < noteCt; m++){
        magnitudes[m][0] = sqrtf((stringsInPhase[m][0] * stringsInPhase[m][0]) + (stringsOutPhase[m][0] * stringsOutPhase[m][0]));
        magnitudes[m][1] = sqrtf((stringsInPhase[m][1] * stringsInPhase[m][1]) + (stringsOutPhase[m][1] * stringsOutPhase[m][1]));
        magnitudes[m][2] = sqrtf((stringsInPhase[m][2] * stringsInPhase[m][2]) + (stringsOutPhase[m][2] * stringsOutPhase[m][2]));

        for(int n = 0; n < 3; n++){

            if(magnitudes[m][n] > max) {

                max = magnitudes[m][n];
                maxLoc = m;
                maxQuality = n;
            }

            avg += magnitudes[m][n];
        }
    }

    avg = avg / (noteCt*3);
    float guessMax = max / avg;
    float threshold = 9.6; //any lower and the reading jumps too much
    //add confidence

   if(guessMax > threshold){

           currNote = noteMap[maxLoc].getName();
           currQuality = maxQuality;
   }

   else {
       currNote = "--";
       currQuality = 1;
   }

    processing = false;
}












