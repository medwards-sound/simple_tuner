/****************************************************************************************************
 * MainActivity.java - Simple Tuner App - Initial Version
 * Michael Edwards 11.19.2019
 *
 * This java file holds all UI code for Simple Tuner App
 * Audio playback and analysis is achieved using c++ code and the AAudio high performance API for
 * Android.(see cmake for more details)
 * jni-bridge.cpp allows communication between native code and ui.
 * Guitar tunings are held in tunings.xml
 *
 * This App is intended as a simple guitar tuner, with a manual as well as auto mode.
 * App allows user to select desired guitar tuning using spinner.
 *
 * Auto mode: allows user to tune the guitar from microphone, highlights when note is locked in.
 * Manual mode: allows user to play sine waves of selected note.
 *
 *
 * NOTE: The algorithm used to find note matches struggles a bit with overtones on low notes.
 * A better reading will occur later in the sound envelope of the string.
 *
 ***************************************************************************************************/


package com.example.simple_tuner;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.CountDownTimer;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.ToggleButton;
import java.util.*;

public class MainActivity extends AppCompatActivity {

    //data
    private boolean tunerMode; //whether tuner is auto or manual (tuning fork) mode
    private String[] tuningStringsFromResource;
    private String currTuning;
    private static final float
        D2 = 73.42f, E2 = 82.41f, G2 = 98.0f, A2 = 110.0f, C3 = 130.81f, D3 = 146.83f, F3 = 174.61f, G3 = 196.0f, A3 = 220.0f,
        B3 = 245.94f, D4 = 293.66f, E4 = 329.63f, Ab2 = 103.83f, Bb3 = 233.08f, Db3 = 138.59f, Eb2 = 77.78f, Eb4 = 311.13f, Gb3 = 185.0f;

    private Map<String,float[]> tuningMap;
    //ui
    private TextView note;
    private TextView noteQuality;
    private Spinner tunings;
    ImageView guitarImage;
    private ToggleButton tunerToggle;
    //timer
    private CountDownTimer tunerTimer; //since timer can't update ui and countdown timer can, this just restarts itself
    private CountDownTimer stringTimer;

    //load native libraries (see cmake for details)
    static{
        System.loadLibrary("native-lib");

    }

    //for use with jni bridge
    private native void startAudioEngine();
    private native void stopAudioEngine(); //for future use
    private native void alterFreq(float freq);
    private native void waveOn(boolean on);
    private native String getNote();
    private native String getNoteQuality();
    private native float getNoteFreq();
    private native void tunerOn(boolean b);
    private native void cleanUp();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tuningMap = new HashMap<>();
        tuningMap.put("Eb_Tuning", new float[]{Eb2, Ab2, Db3, Gb3, Bb3, Eb4});
        tuningMap.put("D_Tuning", new float[]{D2, G2, C3, F3, A3, D4});
        tuningMap.put("Drop_D_Tuning", new float[]{D2, A2, D3, G3, B3, E4});
        tuningMap.put("Standard_Tuning", new float[]{E2, A2, D3, G3, B3, E4});
        tuningMap.put("Open_G_Tuning", new float[]{D2, G2, D3, G3, B3, D4});

        tunerMode = false;
        tunings = findViewById(R.id.spinner_tunings); //spinner
        tuningStringsFromResource = getResources().getStringArray(R.array.tunings_list); //holds all tunings (String values)
        note = findViewById(R.id.txt_note);
        noteQuality = findViewById(R.id.txt_notequality);
        currTuning = "";
        tunerToggle = findViewById(R.id.btn_toggleTuner);
        guitarImage = findViewById(R.id.image_guitar); //center image
        tunerToggle.setOnCheckedChangeListener(toggleChange);
        tunings.setOnItemSelectedListener(ItemChange); //spinner selection listener

        stringTimer = null;
        tunerTimer = null;
        note.setText("--");
        noteQuality.setText(" ");
        getPermissions();
    }

    @Override
    protected void onDestroy(){
        cleanUp();
        super.onDestroy();
    }

    //finish main activity creation
    private void finishCreate(){
        startAudioEngine(); //start AudioEngine
        changeTuning(0);
        note.setText(getNote()); //get note from AnalyzeAudio
        noteQuality.setText(getNoteQuality()); //get note quality (sharp/flat) from AnalyzeAudio
        //waveOn(true);
        changeTunerState(true);
    }

    //get necessary microphone permissions
    private void getPermissions(){
        int p = ContextCompat.checkSelfPermission(getBaseContext(), Manifest.permission.RECORD_AUDIO);
        if(p != PackageManager.PERMISSION_GRANTED)
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 0);
        else
            finishCreate();
    }

    //finish and start audio engine if permission granted
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results){

        switch(requestCode){
            case 0:
            {
                if(results.length > 0 && results[0] == PackageManager.PERMISSION_GRANTED)
                    finishCreate();
                else
                    this.finishAndRemoveTask(); //since the app requires this permission, finish and exit
            }
        }
    }
    /*
    //returns CountDownTimer set roughly to duration of string note
    private CountDownTimer newCountdown(){
        return new CountDownTimer(9000,9000){
            public void onTick(long msToFinish){ }

            //flip image to all green strings
            public void onFinish(){
                flipImage(-1);
                waveOn(false);
                this.cancel();
            }
        };
    }*/

    //returns CountDownTimer that allows polling of tuning data when in auto mode
    private CountDownTimer newCountdownTuner(){
        return new CountDownTimer(1000,70){
            public void onTick(long msToFinish){
                note.setText(getNote());
                noteQuality.setText(getNoteQuality());

                guitarImage.setImageResource(R.drawable.guitarspritegreen);guitarImage.setImageResource(R.drawable.guitarspritegreen);

                if(!getNoteQuality().equals(" ")) return;

                for(int i = 6; i > 0; i--){
                    if(getNoteFreq() != tuningMap.get(currTuning)[6-i]) continue;
                    switch(i){
                        case 6:
                            guitarImage.setImageResource(R.drawable.string6);
                            break;
                        case 5:
                            guitarImage.setImageResource(R.drawable.string5);
                            break;
                        case 4:
                            guitarImage.setImageResource(R.drawable.string4);
                            break;
                        case 3:
                            guitarImage.setImageResource(R.drawable.string3);
                            break;
                        case 2:
                            guitarImage.setImageResource(R.drawable.string2);
                            break;
                        case 1:
                            guitarImage.setImageResource(R.drawable.string1);
                            break;
                        default: guitarImage.setImageResource(R.drawable.guitarspritegreen);
                            break;
                    }
                }
            }

            //keep going
            public void onFinish(){
                guitarImage.setImageResource(R.drawable.guitarspritegreen);
                this.start();
            }
        };
    }

    private void changeTunerState(boolean tunerOn){
        if(tunerOn){
            tunerOn(true);
            tunerMode = true;
            tunerToggle.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalGreen));
            if(stringTimer != null) {
                CountDownTimer temp = stringTimer;
                stringTimer = null;
                temp.onFinish();
            }
            tunerTimer = newCountdownTuner();
            tunerTimer.start();
        }
        else{
            tunerOn(false);
            tunerMode = false;
            tunerToggle.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalBlue));
            tunerTimer.cancel();
        }

        playSound(!tunerOn);
    }

    //toggle button that turns auto mode on or off
    private CompoundButton.OnCheckedChangeListener toggleChange =
            new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {

                    changeTunerState(!b);
                }
    };

    //plays sound based on selected frequency
    private void playSound(boolean p){
        alterFreq(440.0f); //alter frequency of Oscillator used by AudioEngine
        waveOn(p);
    }

    private void changeTuning(int tuning){
        String s = tuningStringsFromResource[tuning]; //get tuning name

        //replace space with _ to get proper tuning value string
        String tuningName = s.replaceAll(" ", "_");

        //get tuning String asset num (tunings.xml string)
        int assetNum = getResources().getIdentifier(tuningName, "string", getPackageName());

        //get String, split on space to get all note String values
        String[] splitStr2 = getResources().getString(assetNum).split("\\s");

        currTuning = tuningName; //set current tuning
        guitarImage.setImageResource(R.drawable.guitarspritegreen); //turn off highlighting for old tuning
    }

    //spinner listener
    private AdapterView.OnItemSelectedListener ItemChange = new AdapterView.OnItemSelectedListener(){
        @Override
        public void onItemSelected(AdapterView<?> a, View v, int i, long l){
            changeTuning(i);
        }

        @Override
        public void onNothingSelected(AdapterView<?> a){
            return;
        }
    };
}
