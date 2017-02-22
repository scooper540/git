void readByte(int level, uint32_t tick, void* userdata, int recallFunction)
{
  //printf("READBYTE\r\n");
  TReadStatus* read_status = (TReadStatus*)userdata;

  //if we start a new byte and it's not the first one and the previous level was high (now LOW), 
  //then don't take the first bit in considenation
  //and if it's a new edge, no continuation from previous byte
  // > avoid having the possibility to have more than 10bits HIGH
  if(read_status->iCount == 0 && read_status->iDataPosition > 0 && level == PI_HIGH && !recallFunction)
  {
    Serial.print("ending bit 0 found, previous was 1 !\r\n");
    read_status->tLastTick += BIT_SIZE_US;
  }

  int32_t length = tick - read_status->tLastTick;
  //check if preambule correct
  
  int countBit = round(((double)length)/BIT_SIZE_US);
/*  Serial.print(countBit);
  Serial.print(" ");
  Serial.println(level);*/
  //printf("POSITION:%d\tREAL LENGTH: %d\tROUNDED LENGTH: %d\r\n", read_status->iCount,length,countBit);
  if(countBit == 0) //skip
    return;

  //first bit
  do
  {
    if(read_status->iCount > 7)
      break;
    if(level == PI_LOW) //we been HIGH before add value
      read_status->iValue += (1 <<  read_status->iCount);
    read_status->iCount++;
    
  }while(--countBit > 0);
 // Serial.println("BYTE VALUE");
  
  if(read_status->iCount >= 8) //8 bit read
  {
    Serial.println(read_status->iValue);
    //printf("BYTE VALUE %d %c\r\n", read_status->iValue, read_status->iValue);
    read_status->data[read_status->iDataPosition++] = read_status->iValue;
    if(read_status->iDataPosition >= STRING_LENGTH)
    {
      Serial.print("String received:");
      Serial.println((char*)read_status->data);
      memset(read_status, 0, sizeof(TReadStatus));
    }
    else
    {
      read_status->iValue = read_status->iCount = 0;
      if(countBit > 0) //rerun the function to increment
      {
        read_status->tLastTick = tick - (countBit * BIT_SIZE_US);
        readByte(level, tick, userdata,1);
      }
    }
  }
  read_status->tLastTick = tick;
}
void readPreambule(int level, uint32_t tick, void* userdata)
{
  TReadStatus* read_status = (TReadStatus*)userdata;

  //preambule mode
  if(!read_status->bReadData)
  {
    if(read_status->tFirstTick == 0)
    {
      read_status->tFirstTick = tick;
    }
    //determine the data length
    int32_t length = tick - read_status->tFirstTick;
    int countBit = round(((double)length)/BIT_SIZE_US);
    Serial.print("LENGTH:");
    Serial.print(length);
    Serial.print(" ");
    Serial.print(countBit);
    Serial.print(" ");
    Serial.println(level);
    if(level == PI_HIGH && length == 0) //start preambule
    {
      //Serial.print("FIRST HIGH preambule found\r\n");
      read_status->tFirstTick = tick;
    }
    else if(level == PI_LOW && (countBit >=10)) 
    {
      Serial.print("FIRST LOW preambule found\r\n");
      read_status->tLastTick = read_status->tFirstTick;
      read_status->bReadData = 1;
    }
    else
    {
      //printf("RESET\r\n");
      //reset
      memset(read_status, 0, sizeof(TReadStatus));
      return;
    }
  }
  if(read_status->bReadData)
  {
    long length = tick - read_status->tLastTick;
    int countBit = round(((double)length)/BIT_SIZE_US);
    //printf("LENGTH: %d\tCOUNTBIT %d\r\n", length, countBit);
    if(!read_status->bPreambuleCorrect)
    {
      //printf("LENGTH PREAMBULE NOT DETERMINED %d\r\n", length);
      if(countBit >= 10)
      {
        Serial.print("preambule found");
        Serial.println(countBit);
        read_status->bPreambuleCorrect = 1;
        read_status->tLastTick = read_status->tFirstTick + (BIT_SIZE_US * 10);
        //activate timer for getting the finished byte
        //gpioSetTimerFuncEx(0,40,&endReadByte, userdata); 
      }
      else
      {
        //printf("RESET DURING DATA\r\n");
        //reset
        memset(read_status, 0, sizeof(TReadStatus));
        return;
      }
    } 
    //if(tick > read_status->tFirstTick + ((BIT_SIZE_US * 8) + TOLERANCE))
    //  return;
    readByte(level, tick, userdata,0);
  } 
}

