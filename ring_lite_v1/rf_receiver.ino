#define STRING_LENGTH 5
#define PI_HIGH 1
#define PI_LOW 0
long BIT_SIZE_US = 1000;
#define FALSE 0
#define TRUE 1


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


bool readByte(int level, uint32_t tick, void* userdata, int recallFunction)
{
	//printf("READBYTE\r\n");
	TReadStatus* read_status = (TReadStatus*)userdata;

	//if we start a new byte and it's not the first one and the previous level was high (now LOW),
	//then don't take the first bit in considenation
	//and if it's a new edge, no continuation from previous byte
	// > avoid having the possibility to have more than 10bits HIGH
	if(read_status->iCount == 0 && read_status->iDataPosition > 0 && level == PI_HIGH && !recallFunction)
	{
		//printf("ending bit 0 found, previous was 1 !");
		read_status->tLastTick += BIT_SIZE_US;
	}

	int32_t length = tick - read_status->tLastTick;
	//check if preambule correct

	int countBit = round(((double)length)/BIT_SIZE_US);
	//printf("POSITION:%d\tREAL LENGTH: %d\tROUNDED LENGTH: %d\r\n", read_status->iCount,length,countBit);
	if(countBit == 0) //skip
		return false;

	//first bit
	do
	{
		if(read_status->iCount > 7)
			break;
		if(level == PI_LOW) //we been HIGH before add value
			read_status->iValue += (1 <<  read_status->iCount);
		read_status->iCount++;

	}while(--countBit > 0);
	//printf("BYTE VALUE %d\r\n", read_status->iValue);

	if(read_status->iCount >= 8) //8 bit read
	{
		//printf("COUNTBIT : %d\r\n", countBit);
		//printf("BYTE VALUE %d %c\r\n", read_status->iValue, read_status->iValue);
		read_status->data[read_status->iDataPosition++] = read_status->iValue;
		if(read_status->iDataPosition >= STRING_LENGTH)
		{
			Serial.print("String received: ");
			Serial.println((char*)read_status->data);
			return true;
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
	return false;
}
bool readPreambule(int gpio, int level, uint32_t tick, void* userdata)
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
		//printf("LENGTH: %d\tCOUNTBIT %d\r\n", length,countBit);
		if(level == PI_HIGH && length == 0) //start preambule
		{
			//printf("FIRST HIGH preambule found\r\n");
			read_status->tFirstTick = tick;
		}
		else if(level == PI_LOW && (countBit >=10))
		{
			//printf("FIRST LOW found\r\n");
			read_status->tLastTick = read_status->tFirstTick;
			read_status->bReadData = 1;
		}
		else
		{
			//printf("RESET\r\n");
			//reset
			memset(read_status, 0, sizeof(TReadStatus));
			return false;
		}
	}
	if(read_status->bReadData)
	{
		int32_t length = tick - read_status->tLastTick;
		int countBit = round(((double)length)/BIT_SIZE_US);
		//printf("LENGTH: %d\tCOUNTBIT %d\r\n", length, countBit);
		if(!read_status->bPreambuleCorrect)
		{
			//printf("LENGTH PREAMBULE NOT DETERMINED %d\r\n", length);
			if(countBit >= 10)
			{
				//printf("preambule found\r\n");
				read_status->bPreambuleCorrect = TRUE;
				read_status->tLastTick = read_status->tFirstTick + (BIT_SIZE_US * 10);
				//activate timer for getting the finished byte
				//gpioSetTimerFuncEx(0,40,&endReadByte, userdata);
			}
			else
			{
				//printf("RESET DURING DATA\r\n");
				//reset
				memset(read_status, 0, sizeof(TReadStatus));
				return false;
			}
		}
		//if(tick > read_status->tFirstTick + ((BIT_SIZE_MS * 8) + TOLERANCE))
		//	return;
		return readByte(level, tick, userdata,0);
	}
	return false;
}

bool read_rf433(int gpio, int timeout_us, unsigned char* data, int maxLength)
{
	TReadStatus stRead;
	memset(&stRead, 0, sizeof(TReadStatus));
	bool bOldInputStatus = 0;
	do
	{
		bool bInput = digitalRead(gpio);
		if(bInput != bOldInputStatus)
		{
			bOldInputStatus = bInput;
			if(readPreambule(gpio, bInput, micros(), &stRead))
			{
				Serial.print("data received: ");
				Serial.println((char*)stRead.data);
				memcpy(data, stRead.data, maxLength);
				return true;
			}
		}
	}while(1);
}

