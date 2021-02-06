#include "AE_GPS.h"

AE_GPS::AE_GPS()
{
}

void AE_GPS::begin(){
  Serial3.begin(9600);
}
// センテンスからフィールドを抜き出す
void AE_GPS::getField(char* buffer, int index)
{
  int sentencePos = 0;
  int fieldPos = 0;
  int commaCount = 0;
  while (sentencePos < SENTENCESIZE)
  {
    if (_sentence[sentencePos] == ',')
    {
      commaCount ++;
      sentencePos ++;
    }
    if (commaCount == index)
    {
      buffer[fieldPos] = _sentence[sentencePos];
      fieldPos ++;
    }
    sentencePos ++;
  }
  buffer[fieldPos] = '\0';
}

// センテンスを受信する
void AE_GPS::getSentence()
{
  while(true)
  {
    static int i = 0;
    if (Serial3.available() != 0 )
    {
      char ch = Serial3.read();
      if (ch != '\n' && i < SENTENCESIZE)
      {
        _sentence[i] = ch;
        i++;
      }
      else
      {
        _sentence[i] = '\0';
        i = 0;
        break;
      }
    }
    delay(0);
  }
}

// 緯度経度情報を更新する
void AE_GPS::refresh()
{
  char field[20];

  while(true){
    getSentence();
    getField(field, 0);
    if (strcmp(field, "$GPRMC") == 0)
    {
      getField(field, 3);  // latitude number
      strcpy(latitude, field);
      getField(field, 4); // N/S
      strcpy(NS, field);
    
      getField(field, 5);  // longitude number
      strcpy(longitude, field);
      getField(field, 6);  // E/W
      strcpy(EW, field);
      break;
    }
    delay(0);
  }
}
