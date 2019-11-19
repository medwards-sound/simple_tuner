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
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.ToggleButton;

public class MainActivity extends AppCompatActivity {

    //data
    private boolean autoMode; //whether tuner is auto or manual (tuning fork) mode
    private String[] ary_tuning;
    private String currTuning;
    float A2, B3, D3, C3, E2, E4, G3, G2, D2, Ab2, Bb3, Eb2, Eb4, Gb3, Db3, F3, D4, A3; //notes that can be played and matched
    float[] tuneE, tuneEb, tuneD, tuneDropD, tuneOpenG, tuneCurrent; //each tuning available

    //ui
    private TextView note;
    private TextView noteQuality;
    private Spinner tunings;
    ImageView guitarImage;
    private Button button1, button2, button3, button4, button5, button6;
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        autoMode = false;

        tunings = findViewById(R.id.spinner_tunings); //spinner
        ary_tuning = getResources().getStringArray(R.array.tunings_list); //holds all tunings (String values)

        note = findViewById(R.id.txt_note);
        noteQuality = findViewById(R.id.txt_notequality);

        currTuning = "";

        //guitar string buttons
        button1 = findViewById(R.id.btn_string1);
        button2 = findViewById(R.id.btn_string2);
        button3 = findViewById(R.id.btn_string3);
        button4 = findViewById(R.id.btn_string4);
        button5 = findViewById(R.id.btn_string5);
        button6 = findViewById(R.id.btn_string6);

        tunerToggle = findViewById(R.id.btn_toggleTuner);

        guitarImage = findViewById(R.id.image_guitar); //center image

        //set button listeners
        button6.setOnClickListener(string6);
        button5.setOnClickListener(string5);
        button4.setOnClickListener(string4);
        button3.setOnClickListener(string3);
        button2.setOnClickListener(string2);
        button1.setOnClickListener(string1);

        tunerToggle.setOnCheckedChangeListener(toggleChange);

        tunings.setOnItemSelectedListener(ItemChange); //spinner selection listener

        loadFreqValues();
        setTuningArrays();

        stringTimer = null;
        tunerTimer = null;

        note.setText("--");
        noteQuality.setText(" ");

        getPermissions();
    }

    //finish main activity creation
    private void finishCreate(){

        startAudioEngine(); //start AudioEngine
        note.setText(getNote()); //get note from AnalyzeAudio
        noteQuality.setText(getNoteQuality()); //get note quality (sharp/flat) from AnalyzeAudio
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

    //returns CountDownTimer set roughly to duration of string note
    private CountDownTimer newCountdown(){

        return new CountDownTimer(9000,9000){
            public void onTick(long msToFinish){ }

            //flip image to all green strings
            public void onFinish(){
                flipImage(-1);
                waveOn(false);
            }
        };
    }

    //returns CountDownTimer that allows polling of tuning data when in auto mode
    private CountDownTimer newCountdownTuner(){

        return new CountDownTimer(1000,70){
            public void onTick(long msToFinish){
                note.setText(getNote());
                noteQuality.setText(getNoteQuality());

                flipImage(-1);
                if(getNoteQuality().equals(" ")){

                    if(getNoteFreq() == tuneCurrent[0])
                        flipImage(6);
                    else if(getNoteFreq() == tuneCurrent[1])
                        flipImage(5);
                    else if(getNoteFreq() == tuneCurrent[2])
                        flipImage(4);
                    else if(getNoteFreq() == tuneCurrent[3])
                        flipImage(3);
                    else if(getNoteFreq() == tuneCurrent[4])
                        flipImage(2);
                    else if(getNoteFreq() == tuneCurrent[5])
                        flipImage(1);
                }

            }

            //keep going
            public void onFinish(){
                this.start();
            }
        };
    }

    //toggle button that turns auto mode on or off
    private CompoundButton.OnCheckedChangeListener toggleChange =
            new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {

                    if(b){
                        tunerOn(true);
                        autoMode = true;
                        tunerToggle.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalBlue));
                        if(stringTimer != null) {
                            stringTimer.onFinish();
                            stringTimer.cancel();
                        }

                        tunerTimer = newCountdownTuner();
                        tunerTimer.start();

                    }
                    else{
                        tunerOn(false);
                        autoMode = false;
                        tunerToggle.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalGreen));
                        tunerTimer.cancel();
                    }
                }
    };

    //sets all tuning arrays (used to highlight when in tune note found)
    private void setTuningArrays(){

        tuneE = new float[]{E2, A2, D3, G3, B3, E4};
        tuneEb = new float[]{Eb2, Ab2, Db3, Gb3, Bb3, Eb4};
        tuneD = new float[]{D2, G2, C3, F3, A3, D4};
        tuneOpenG = new float[]{D2, G2, D3, G3, B3, D4};
        tuneDropD = new float[]{D2, A2, D3, G3, B3, E4};
    }

    //flips to desired guitar image for current note and highlights note pressed,
    //or in the case of auto mode, will highlight when note locks in on a valid
    //note in the currently selected tuning
    public void flipImage(int stringNum){

        //cancel timer
        if(stringTimer != null)
           stringTimer.cancel();

       // button6.setBackgroundColor(getResources().getColor(R.color.terminalGreen));
        button6.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalGreen));
        button5.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalGreen));
        button4.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalGreen));
        button3.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalGreen));
        button2.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalGreen));
        button1.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalGreen));


        //flip desired image and highlight desired button
        switch(stringNum){

            case 6: guitarImage.setImageResource(R.drawable.string6);
                button6.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalBlue));
                break;

            case 5: guitarImage.setImageResource(R.drawable.string5);
                button5.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalBlue));
                break;

            case 4: guitarImage.setImageResource(R.drawable.string4);
                button4.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalBlue));
                break;

            case 3: guitarImage.setImageResource(R.drawable.string3);
                button3.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalBlue));
                break;

            case 2: guitarImage.setImageResource(R.drawable.string2);
                button2.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalBlue));
                break;

            case 1: guitarImage.setImageResource(R.drawable.string1);
                button1.setBackgroundColor(ContextCompat.getColor(getBaseContext(), R.color.terminalBlue));
                break;

            default: guitarImage.setImageResource(R.drawable.guitarspritegreen);
                break;
        }

        //stringTimer.cancel();
        if(!(stringNum == -1) && !autoMode) {
            stringTimer = newCountdown(); //get new countdown
            stringTimer.start(); //start countdown
        }

    }

    //listener for button 6
    private View.OnClickListener string6 = new View.OnClickListener(){

        @Override
        public void onClick(View v){

            if(!autoMode) {
                playSound(tuneCurrent[0]); //play note
                flipImage(6); //highlight note playing
            }
        }
    };

    //listener for button 5
    private View.OnClickListener string5 = new View.OnClickListener(){

        @Override
        public void onClick(View v){

            if(!autoMode) {
                playSound(tuneCurrent[1]); //play note
                flipImage(5); //highlight note playing
            }
        }
    };

    //listener for button 4
    private View.OnClickListener string4 = new View.OnClickListener(){

        @Override
        public void onClick(View v){

            if(!autoMode) {
                playSound(tuneCurrent[2]); //play note
                flipImage(4); //highlight note playing
            }
        }
    };

    //listener for button 3
    private View.OnClickListener string3 = new View.OnClickListener(){

        @Override
        public void onClick(View v){

            if(!autoMode) {
                playSound(tuneCurrent[3]); //play note
                flipImage(3); //highlight note playing
            }
        }
    };

    //listener for button 2
    private View.OnClickListener string2 = new View.OnClickListener(){

        @Override
        public void onClick(View v){

            if(!autoMode) {
                playSound(tuneCurrent[4]); //play note
                flipImage(2); //highlight note playing
            }
        }
    };

    //listener for button 1
    private View.OnClickListener string1 = new View.OnClickListener(){

        @Override
        public void onClick(View v){

            if(!autoMode) {
                playSound(tuneCurrent[5]); //play note
                flipImage(1); //highlight note playing
            }
        }
    };

    //plays sound based on selected frequency
    private void playSound(float freq){

        alterFreq(freq); //alter frequency of Oscillator used by AudioEngine
    }


    //load all note values into for use with oscillator and for reference
    private void loadFreqValues(){

        //load all notes
        D2 = 73.42f;
        E2 = 82.41f;
        G2 = 98.0f;
        A2 = 110.0f;
        C3 = 130.81f;
        D3 = 146.83f;
        F3 = 174.61f;
        G3 = 196.0f;
        A3 = 220.0f;
        B3 = 245.94f;
        D4 = 293.66f;
        E4 = 329.63f;

        Ab2 = 103.83f;
        Bb3 = 233.08f;
        Db3 = 138.59f;
        Eb2 = 77.78f;
        Eb4 = 311.13f;
        Gb3 = 185.0f;
    }

    //spinner listener
    private AdapterView.OnItemSelectedListener ItemChange = new AdapterView.OnItemSelectedListener(){
        @Override
        public void onItemSelected(AdapterView<?> a, View v, int i, long l){


            String s = ary_tuning[i]; //get tuning name

            //replace space with _ to get proper tuning value string
            String tuningName = s.replaceAll(" ", "_");

            //get tuning String asset num (tunings.xml string)
            int assetNum = getResources().getIdentifier(tuningName, "string", getPackageName());

            //get String, split on space to get all note String values
            String[] splitStr2 = getResources().getString(assetNum).split("\\s");

            //set buttons to proper guitar string names
            button6.setText(splitStr2[0]);
            button5.setText(splitStr2[1]);
            button4.setText(splitStr2[2]);
            button3.setText(splitStr2[3]);
            button2.setText(splitStr2[4]);
            button1.setText(splitStr2[5]);

            currTuning = tuningName; //set current tuning

            //set to proper note array based on current tuning
            switch(currTuning){

                case "Eb_Tuning": tuneCurrent = tuneEb;
                    break;

                case "D_Tuning": tuneCurrent = tuneD;
                    break;

                case "Drop_D_Tuning": tuneCurrent = tuneDropD;
                    break;

                case "Open_G_Tuning": tuneCurrent = tuneOpenG;
                    break;

                default: tuneCurrent = tuneE;
                    break;
            }

            flipImage(-1); //turn off highlighting for old tuning
        }

        @Override
        public void onNothingSelected(AdapterView<?> a){
            return;
        }
    };
}
