#include <Arduino.h>

/*
  Send data over RF433 with my algo Christophe Petit
 */

const int interval = 1;
const int pinOutput = 12;
  const unsigned char test1[] = {'a','b','c','d','e'};
  const unsigned char test2[] = {'A',150,'C',255,231};
void sendBytes(const unsigned char* s, unsigned int length)
{
  for(int i = 0;i < length; i++)
  {
    char c = s[i];
    for(int j = 0; j < 8; j++)
    { //get bit by bit
      int val = (c >> j) & 0x01;
     // Serial.printf("CHAR %c BIT NUMBER %d WRITE BIT %d\n", c,j,val);
      digitalWrite(pinOutput,  val);   
      delay(interval);
    }
    //send LOW value at the end if last is 1
    if(((c >> 7) & 0x01) == 1)
    {
      digitalWrite(pinOutput, LOW);
      delay(interval);
    }
  }
  
}
void sendPreambule()
{
  //send low first to reset the other side
  digitalWrite(pinOutput, LOW);
  delay(interval);
  digitalWrite(pinOutput, HIGH);
  delay(interval);
  digitalWrite(pinOutput, LOW);
  delay(interval);
  digitalWrite(pinOutput, HIGH);
  delay(interval * 10);         
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(pinOutput, OUTPUT);
  Serial.begin(9600);  
}

// the loop function runs over and over again forever
void loop() {

  sendPreambule();
  sendBytes(test1,5);
  sendPreambule();
  sendBytes(test2,5);
}

