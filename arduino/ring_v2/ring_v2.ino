/**
  @file    fmrx_demo.ino
  @author  www.elechouse.com
  @brief   example of FMRX_MODULE
  
    For this demo, input command format like &xxxx to set select band,
 (eg:if you want to set FM 95.6MHz,then input &0956 and send).
  
  @section  HISTORY
  
  V1.0 initial version
  
    Copyright (c) 2012 www.elechouse.com  All right reserved.
*/

#include "FMTX.h"
#include <SD.h>                      // need to include the SD library
#define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328
#include <TMRpcm.h>           //  also need to include this library...
 #include <arduino.h>
#include <Wire.h>
#include <radio.h>
#include <RDA5807M.h>
RDA5807M radio;    ///< Create an instance of a RDA5807 chip radio
TMRpcm tmrpcm;   // create an object for use in this sketch
float fm_freq = 89.3;  // Here set the default FM frequency

#define FIX_BAND     RADIO_BAND_FM   ///< The band that will be tuned by this sketch is FM.
#define FIX_STATION  8930            ///< The station that will be tuned by this sketch is 89.30 MHz.
#define FIX_VOLUME   8            ///< The volume that will be set by this sketch is level 4.



// TONES  ==========================================
// Start by defining the relationship between 
//       note, period, &  frequency. 
#define  c     3830    // 261 Hz 
#define  d     3400    // 294 Hz 
#define  e     3038    // 329 Hz 
#define  f     2864    // 349 Hz 
#define  g     2550    // 392 Hz 
#define  a     2272    // 440 Hz 
#define  b     2028    // 493 Hz 
#define  C     1912    // 523 Hz 
// Define a special note, 'R', to represent a rest
#define  R     0


// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
int speakerOut = 9;

// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note) 
int melody[] = {  C, c };
int beats[]  = { 16, 16 }; 
int MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration  = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has 
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(tone_ / 2);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    } 
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                 
}

void setup(void)
{
  
  Serial.begin(9600);
  Serial.print("FM-TX Demo\r\n");
  /**
  Initial, set FM channel and select your area:
 USA
 EUROPE
 JAPAN
 AUSTRALIA
 CHINA
  */
  //fmtx_init(fm_freq, EUROPE);
  Serial.print("Channel:");
  Serial.print(fm_freq, 1);
  Serial.println("MHz");

    // Initialize the Radio 
  radio.init();

  // Enable information to the Serial port
  radio.debugEnable();

  // Set all radio setting to the fixed values.
  radio.setBandFrequency(FIX_BAND, FIX_STATION);
  radio.setVolume(FIX_VOLUME);
  radio.setMono(false);
  radio.setMute(false);
  radio.setBassBoost(false);
  pinMode(A0,INPUT);
  
pinMode(speakerOut, OUTPUT);


  
  /*
  tmrpcm.speakerPin = 9; //11 on Mega, 9 on Uno, Nano, etc
 
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
  return;   // don't do anything more if not
  }
 tmrpcm.volume(1);
 tmrpcm.play("1.wav"); //the sound file "1" will play each time the arduino powers up, or is reset
*/
}

void loop(void)
{
  int melodyActivated = false;
  do
  {
    if(melodyActivated == true)
    {
      pinMode(speakerOut, OUTPUT);
    for (int i=0; i<MAX_COUNT; i++) 
    {
      tone_ = melody[i];
      beat = beats[i];

      duration = beat * tempo; // Set up timing

      playTone(); 
    // A pause between notes...
      delayMicroseconds(pause);
    }
    }
    else
      pinMode(speakerOut, INPUT);
 /*  int aaaa = analogRead(A0);
    Serial.print("b=");
    Serial.print(aaaa);
    Serial.println("");
   */ //if (DEBUG) 
  /* { // If debugging, report loop, tone, beat, and duration
      Serial.print(i);
      Serial.print(":");
      Serial.print(beat);
      Serial.print(" ");    
      Serial.print(tone_);
      Serial.print(" ");
      Serial.println(duration);
    }*/
  
  if(Serial.available())
  {
    switch(Serial.read())
    {
      //activate or disable melody
      case '1':
        melodyActivated = true;
        break;
      case '0':
        melodyActivated = false;
        break;
      case 'm':
          pinMode(6,INPUT);
        break;  
       case 'n':
        pinMode(6,OUTPUT);
        digitalWrite(6, LOW);
        break;  
    }
  }
  }while(true);
  
  /*
  int counter = 0;
  int sense = 1;
  do
  {
    int i = analogRead(10);
    Serial.print("i=");
    Serial.print(i);
    Serial.println("");
   /*  analogWrite(9,counter);
     if(sense == 1)
     {
         if(counter++ >= 255)
          {
            counter = 254;
            sense = 0;
            
          }
      
     }
     else
     {
        if(counter-- <= 0)
        {
          counter = 0;
          sense = 1;
        }   
     }*/
    /* if(tmrpcm.isPlaying() == 0)
     {
      Serial.println("finished");
       tmrpcm.play("1.wav"); //the sound file "1" will play each time the arduino powers up, or is reset
     }
    delay(100);
  }while(true);*/
}
