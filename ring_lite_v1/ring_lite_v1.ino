#include "Arduino.h"

/*
 * PIN ASSIGNEMENT
 * 2 & 3 : ID_LOW and ID_HIGH. No jumper = master
 * For master : pin 4 and 5 = input for dest : 4 -> id 1 and 5 -> id 2
 * For slave : pin 4 : DEL when ring
 * 6 RF Receiver
 * 7 RF Sender
 */

#define ID_LOW 		2
#define ID_HIGH 	3
#define INPUT1 		4
#define INPUT2 		5
#define LED_RING 	4
#define RF_RX		6
#define	RF_TX		7

//timeout 1second
#define TIMEOUT_RF_RX		1000*1000

bool bIsMaster = true;
int iMyID = 0;
void setup()
{
	Serial.begin(115200);
	Serial.print("Starting ring_lite_v1\r\n");


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
	}
	else
	{
		pinMode(LED_RING, OUTPUT);
		digitalWrite(LED_RING, LOW);
	}

	pinMode(RF_RX, INPUT);
	pinMode(RF_TX, OUTPUT);
}


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
						delay(3000);
						digitalWrite(LED_RING, LOW);
					}
				}

			}
		}
	}while(1);
}