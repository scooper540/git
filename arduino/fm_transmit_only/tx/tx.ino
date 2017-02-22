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
float fm_freq = 89.3;  // Here set the default FM frequency

#define FIX_BAND     RADIO_BAND_FM   ///< The band that will be tuned by this sketch is FM.
#define FIX_STATION  8930            ///< The station that will be tuned by this sketch is 89.30 MHz.
#define FIX_VOLUME   15            ///< The volume that will be set by this sketch is level 4.

// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
int speakerOut = 9;

// Set overall tempo
long time_note = 20000;
long delayBetweenNotes = 20;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone(int value) 
{
  long elapsed_time = 0;
  int duration = time_note; // Set up timing
  if (value > 0) 
  {
      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(duration * 0.75);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(duration * 0.25);
  }
  else { // Rest beat; loop times delay
      delay(delayBetweenNotes);  
    }  
    delay(delayBetweenNotes);
                                   
}
void sendBytes(const unsigned char* s, unsigned int length)
{
  for(int i = 0;i < length; i++)
  {
    char c = s[i];
    for(int j = 0; j < 8; j++)
    { //get bit by bit
      int val = (c >> j) & 0x01;
     // Serial.printf("CHAR %c BIT NUMBER %d WRITE BIT %d\n", c,j,val);
      playTone(val);
    }
    //send LOW value at the end if last is 1
    if(((c >> 7) & 0x01) == 1)
    {
      playTone(0);
    }
  }
  
}
void sendPreambule()
{
  //send low first to reset the other side
  playTone(0);
  playTone(1);
  playTone(0);
  for(int i = 0; i < 10;i++)
    playTone(1);
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
  fmtx_init(fm_freq, EUROPE);
  Serial.print("Channel:");
  Serial.print(fm_freq, 1);
  Serial.println("MHz");
/*
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
  int melodyActivated = true;
  do
  {
    if(melodyActivated == true)
    {
      pinMode(speakerOut, OUTPUT);
      const unsigned char test1[] = {'a','b','c','d','e'};
      sendPreambule();
      sendBytes(test1,5);
       playTone(0);
       playTone(1);
      /*playTone(1);
      playTone(1);
      playTone(1);*/
    // A pause between notes...
      delay(1000);
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
