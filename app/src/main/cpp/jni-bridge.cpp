#include <jni.h>
#include <android/input.h>
#include <string>
#include "AudioEngine.h"

static AudioEngine* audioEngine = new AudioEngine();

extern "C"{

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_alterFreq(JNIEnv* env, jobject /* this */, jfloat freq){

        audioEngine->getSynthStream()->getOscillator()->alterFreq((float)freq);
        audioEngine->getSynthStream()->getOscillator()->oscillatorOn(true);
    }

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_startAudioEngine(JNIEnv* env, jobject /* this */){

        audioEngine->start();
    }

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_stopAudioEngine(JNIEnv* env, jobject /* this */){

        audioEngine->stop();
    }

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_waveOn(JNIEnv* env, jobject /*this */, jboolean on){

        audioEngine->getSynthStream()->getOscillator()->oscillatorOn((bool)on);
    }

    JNIEXPORT jstring JNICALL
    Java_com_example_simple_1tuner_MainActivity_getNote(JNIEnv* env, jobject /* this */){

        return env->NewStringUTF(audioEngine->getAnalysisStream()->getAnalyzeAudio()->getCurrNote().c_str());
    }

    JNIEXPORT jstring JNICALL
    Java_com_example_simple_1tuner_MainActivity_getNoteQuality(JNIEnv* env, jobject /* this */){

        return env->NewStringUTF(audioEngine->getAnalysisStream()->getAnalyzeAudio()->getQuality().c_str());
    }

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_tunerOn(JNIEnv* env, jobject /* this */, jboolean on){

        audioEngine->getAnalysisStream()->getAnalyzeAudio()->setTunerOn((bool)on);
    }

    JNIEXPORT jfloat JNICALL
    Java_com_example_simple_1tuner_MainActivity_getNoteFreq(JNIEnv* env, jobject /* this */){

        return (jfloat) audioEngine->getAnalysisStream()->getAnalyzeAudio()->getNoteFreq();
    }

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_cleanUp(JNIEnv* env, jobject /* this */){

        audioEngine->stop();
        delete audioEngine;
    }
}