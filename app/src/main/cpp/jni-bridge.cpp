#include <jni.h>
#include <android/input.h>
#include <string>
#include "AudioEngine.h"

static AudioEngine* audioEngine = new AudioEngine();

extern "C"{

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_alterFreq(JNIEnv* env, jobject /* this */, jfloat freq){

        audioEngine->getOscillator()->alterFreq((float)freq);
        audioEngine->getOscillator()->oscillatorOn(true);
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

        audioEngine->getOscillator()->oscillatorOn(on);
    }

    JNIEXPORT jstring JNICALL
    Java_com_example_simple_1tuner_MainActivity_getNote(JNIEnv* env, jobject /* this */){

        return env->NewStringUTF(audioEngine->getAnalyzeAudio()->getCurrNote().c_str());
    }

    JNIEXPORT jstring JNICALL
    Java_com_example_simple_1tuner_MainActivity_getNoteQuality(JNIEnv* env, jobject /* this */){

        return env->NewStringUTF(audioEngine->getAnalyzeAudio()->getQuality().c_str());
    }

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_tunerOn(JNIEnv* env, jobject /* this */, jboolean on){

        audioEngine->getAnalyzeAudio()->setTunerOn((bool)on);
    }

    JNIEXPORT jfloat JNICALL
    Java_com_example_simple_1tuner_MainActivity_getNoteFreq(JNIEnv* env, jobject /* this */){

        return (jfloat) audioEngine->getAnalyzeAudio()->getNoteFreq();
    }

    JNIEXPORT void JNICALL
    Java_com_example_simple_1tuner_MainActivity_cleanUp(JNIEnv* env, jobject /* this */){

        audioEngine->stop();
        delete audioEngine;
    }
}