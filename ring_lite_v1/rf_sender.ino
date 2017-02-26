
/*
  Send data over RF433 with my algo Christophe Petit
 */

const int interval = 1;

void sendBytes(const unsigned char* s, unsigned int length)
{
  for(int i = 0;i < length; i++)
  {
    char c = s[i];
    for(int j = 0; j < 8; j++)
    { //get bit by bit
      int val = (c >> j) & 0x01;
     // Serial.printf("CHAR %c BIT NUMBER %d WRITE BIT %d\n", c,j,val);
      digitalWrite(RF_TX,  val);
      delay(interval);
    }
    //send LOW value at the end if last is 1
    if(((c >> 7) & 0x01) == 1)
    {
      digitalWrite(RF_TX, LOW);
      delay(interval);
    }
  }

}
void sendPreambule()
{
	Serial.println("send preambule");
  //send low first to reset the other side
  digitalWrite(RF_TX, LOW);
  delay(interval);
  digitalWrite(RF_TX, HIGH);
  delay(interval);
  digitalWrite(RF_TX, LOW);
  delay(interval);
  digitalWrite(RF_TX, HIGH);
  delay(interval * 10);
}
