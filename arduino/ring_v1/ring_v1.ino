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

//#include "FMTX.h"
//#include <SD.h>                      // need to include the SD library
//#define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328
//#include <TMRpcm.h>           //  also need to include this library...
#include <arduino.h>
//#include <Wire.h>
#include <radio.h>
#include <RDA5807M.h>
RDA5807M radio;    ///< Create an instance of a RDA5807 chip radio
//TMRpcm tmrpcm;   // create an object for use in this sketch
float fm_freq = 91.3;  // Here set the default FM frequency

#define FIX_BAND     RADIO_BAND_FM   ///< The band that will be tuned by this sketch is FM.
#define FIX_STATION  8930            ///< The station that will be tuned by this sketch is 89.30 MHz.
#define FIX_VOLUME   5           ///< The volume that will be set by this sketch is level 4.



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

#define STRING_LENGTH 5
#define PI_HIGH 1
#define PI_LOW 0
typedef struct
{
  uint32_t  tLastTick;
  uint32_t  tFirstTick;
  int     bReadData;
  int     bPreambuleCorrect;
  int     iValue;
  int     iCount;
  unsigned char data[STRING_LENGTH + 1];
  int     iDataPosition;
}TReadStatus;
// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
int speakerOut = 9;

int pinActivateSound = 10;
int pinData = A0;
float voltageMin = 0.4;
long BIT_SIZE_US = 40000;



void setup(void)
{
  
  Serial.begin(115200);
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
  radio.setSoftMute(true);
 pinMode(pinData,INPUT);
  
pinMode(speakerOut, OUTPUT);
        pinMode(pinActivateSound,OUTPUT);


  
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
  int oldValue = 0;
  bool is_synchronized = false;
  long lCount = 0;
  long oldTick = 0;
   TReadStatus stRead;
   memset(&stRead, 0, sizeof(TReadStatus));
  do
  {
    do
    {
      int newValue = 0;
      float voltage = 0;
    
      
        newValue = analogRead(pinData);
        voltage = newValue * (5.0 / 1023.0);
        newValue = voltage > voltageMin;
     
      
      
     // Serial.println(voltage);
      
      long tick = micros();
      if((tick - oldTick) > BIT_SIZE_US || stRead.tFirstTick == 0)
      { 
        oldTick = tick;
        while((micros() - tick) < (BIT_SIZE_US / 5) && !newValue && stRead.tFirstTick != 0)
        {
          newValue = analogRead(pinData);
          voltage = newValue * (5.0 / 1023.0);
          newValue = voltage > voltageMin;
         // oldTick = micros();
          delayMicroseconds(100);
        }
        //Serial.print(newValue);
        if(newValue != oldValue)
        {
          oldValue = newValue;
          readPreambule(newValue,oldTick,&stRead);
        }
        //if((tick - stRead.tFirstTick) > BIT_SIZE_US * 15)
        //     memset(&stRead, 0, sizeof(TReadStatus));
      }
      delayMicroseconds(100);
      
  }while(stRead.tFirstTick > 0);
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
      case '2':
        digitalWrite(pinActivateSound,LOW);
        break;
       case '3':
        digitalWrite(pinActivateSound,HIGH);
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

