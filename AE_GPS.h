#ifndef AE_GPS_H
#define AE_GPS_H

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define SENTENCESIZE 80 // sentence size

class AE_GPS{
public:
  AE_GPS(); //引数はRXおよびTXのPIN番号
  void begin();
  char latitude[20]; // 緯度 dddmm.mmmm
  char longitude[20]; // 経度 dddmm.mmmm
  char NS[2]; // 北緯or南緯
  char EW[2]; // 東経or西経

  void refresh(); // 位置情報を更新する

private:
  char _sentence[SENTENCESIZE];

  void getSentence();
  void getField(char* buffer, int index);
};

#endif // AE_GPS_H
