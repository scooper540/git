#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2017-02-26 20:15:14

#include "Arduino.h"
#include "Arduino.h"
bool readByte(int level, uint32_t tick, void* userdata, int recallFunction) ;
bool readPreambule(int gpio, int level, uint32_t tick, void* userdata) ;
bool read_rf433(int gpio, int timeout_us, unsigned char* data, int maxLength) ;
void sendBytes(const unsigned char* s, unsigned int length) ;
void sendPreambule() ;
void setup() ;
void checkIfRingAccepted(int iID) ;
void loop() ;

#include "ring_lite_v1.ino"

#include "rf_receiver.ino"
#include "rf_sender.ino"

#endif