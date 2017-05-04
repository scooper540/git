#define __FM_DEBUG

#include "Arduino.h"
#include "lib/radio.h"
#include "lib/RDA5807M.h"
#include "lib/FMTX.h"

/*
 * PIN ASSIGNEMENT
 * 2 & 3 : ID_LOW and ID_HIGH. No jumper = master
 * For master : pin 4 and 5 = input for dest : 4 -> id 1 and 5 -> id 2
 * For slave : pin 4 : DEL when ring
 * 6 RF Receiver
 * 7 RF Sender
 * A5 : SCL to FM Transmit
 * A4 : SDA to FM Transmit
 *
 * RF Receiver MX-TX-5V ? -> VCC 5v
 * RF Transmitter FS1000A -> VCC 5v
 * FM Receiver RDA5807M	  -> VCC 3.3V
 * FM Tx Elechouse FM TX  -> VCC 5v
 */

#define _DEBUG_
#define _TEST_MODE_


#define ID_LOW 		2
#define ID_HIGH 	3
#define INPUT1 		4
#define INPUT2 		5
#define LED_RING 	4
#define RF_RX		6
#define	RF_TX		8

//timeout 1second
#define TIMEOUT_RF_RX		1000*1000


// FM stuff
#define MASTER_FM_RX		9900
#define MASTER_FM_TX		10200
#define SLAVE_FM_RX			MASTER_FM_TX
#define SLAVE_FM_TX			MASTER_FM_RX


bool bIsMaster = true;
int iMyID = 0;

RDA5807M oFMRx;

void setup()
{
	Serial.begin(115200);
	Serial.print("Starting ring_lite_v1\r\n");

	pinMode(7,OUTPUT);
	digitalWrite(7, 1);
	pinMode(ID_LOW,INPUT_PULLUP);
	pinMode(ID_HIGH,INPUT_PULLUP);
	iMyID = ((digitalRead(ID_LOW) == 0) ? 1 : 0);
	iMyID += ((digitalRead(ID_HIGH) == 0) ? 2 : 0);
	bIsMaster = iMyID == 0;
	Serial.print("My ID: ");
	Serial.print(iMyID);
	Serial.print("Is Master: \r\n");
	Serial.print(bIsMaster);
	Serial.println();
	if(bIsMaster)
	{
		pinMode(INPUT1, INPUT_PULLUP);
		pinMode(INPUT2, INPUT_PULLUP);

		oFMRx.init();
		delay(10);
		oFMRx.setBandFrequency(RADIO_BAND_FM, MASTER_FM_RX);
		delay(10);
		oFMRx.setMono(false);
		delay(10);
		oFMRx.setMute(false);
		delay(10);
		oFMRx.setVolume(15);
		delay(10);
		Serial.println("FX RX initialized");
		Serial.println(oFMRx.getFrequency());
		fmtx_init((float)(MASTER_FM_TX / 100), EUROPE);
		Serial.println("FX TX initialized");
		Serial.println((float)(MASTER_FM_TX / 100));

	}
	else
	{
		pinMode(LED_RING, OUTPUT);
		digitalWrite(LED_RING, LOW);

		oFMRx.init();
		delay(10);
		oFMRx.setBandFrequency(RADIO_BAND_FM, SLAVE_FM_RX);
		delay(10);
		oFMRx.setMono(false);
		delay(10);
		oFMRx.setMute(false);
		delay(10);
		oFMRx.setVolume(15);
		delay(10);
		Serial.println("FX RX initialized");
		Serial.println(oFMRx.getFrequency());
		fmtx_init((float)(SLAVE_FM_TX / 100), EUROPE);
		Serial.println("\r\nFX TX initialized");
		Serial.println((float)(SLAVE_FM_TX / 100));
	}

	pinMode(RF_RX, INPUT);
	pinMode(RF_TX, OUTPUT);

}

#ifdef _TEST_MODE_
void startTestMode()
{
	unsigned char msg[6];
	memset(&msg, 0, sizeof(msg));
	if(bIsMaster) // check for button pressed
	{
		msg[0] = 1; //to ID 1
		msg[1] = 1; //action 1 -> ring
		sendPreambule();
		sendBytes(msg,sizeof(msg));
	}
	else
	{
		if(read_rf433(RF_RX, TIMEOUT_RF_RX, msg, sizeof(msg)))
		{
			Serial.println((char*)msg);
			if(msg[0] == 1)
			{
				Serial.println("New Message for me");
				if(msg[1] == 1) //ring
				{
					digitalWrite(LED_RING, HIGH);
					delay(300);
					digitalWrite(LED_RING, LOW);
				}
			}
		}
	}
}
#endif
void checkIfRingAccepted(int iID)
{
	unsigned char msg[6];
	memset(&msg, 0, sizeof(msg));
	if(read_rf433(RF_RX, TIMEOUT_RF_RX, msg, sizeof(msg)))
	{
		Serial.println((char*)msg);
		if(msg[0] == iMyID)
		{
			Serial.println("New Message for me");
			if(msg[1] == iID) //sender
			{
				if(msg[2] == 1) //ring accepted
				{
					// start audio
					Serial.println("ring accepted, start audio");
					oFMRx.setMute(false);
					delay(10000);
					oFMRx.setMute(true);
				}
			}
		}
	}

}

void loop()
{
	uint32_t tInputPressed = 0;

	do
	{
#ifdef _TEST_MODE_
		startTestMode();
		return;
#endif
		unsigned char msg[6];
		memset(&msg, 0, sizeof(msg));
		if(bIsMaster) // check for button pressed
		{
			if(!digitalRead(INPUT1) && ((millis() - tInputPressed) > 5000)) // 5 seconds
			{

				Serial.print("INPUT1 pressed\r\n");

				msg[0] = 1; //to ID 1
				msg[1] = 1; //action 1 -> ring
				sendPreambule();
				sendBytes(msg,sizeof(msg));
				tInputPressed = millis();
				Serial.println(tInputPressed);
				checkIfRingAccepted(1);
			}
			else if(!digitalRead(INPUT2)&& ((millis() - tInputPressed) > 5000)) // 5 seconds
			{
				Serial.print("INPUT2 pressed\r\n");
				msg[0] = 2; //to ID 1
				msg[1] = 1; //action 1 -> ring
				sendPreambule();
				sendBytes(msg,sizeof(msg));
				tInputPressed = millis();
				Serial.println(tInputPressed);
			}
		}
		else // slave, just read the rf receiver
		{
			if(read_rf433(RF_RX, TIMEOUT_RF_RX, msg, sizeof(msg)))
			{
				Serial.println((char*)msg);
				if(msg[0] == 1)
				{
					Serial.println("New Message for me");
					if(msg[1] == 1) //ring
					{
						digitalWrite(LED_RING, HIGH);
						//send accepted
						msg[0] = 0; //to master
						msg[1] = iMyID; //myid
						msg[2] = 1; //accept
						sendPreambule();
						sendBytes(msg,sizeof(msg));
						oFMRx.setMute(false);
						delay(10000);
						digitalWrite(LED_RING, LOW);
						oFMRx.setMute(true);
					}
				}

			}
		}
	}while(1);
}
