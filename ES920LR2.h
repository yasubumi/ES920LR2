#ifndef ES920LR2_H
#define ES920LR2_H

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/* ---------------
 * 色々定義
 -----------------*/
// 帯域幅
#define BW_62_5KHZ  3
#define BW_125KHZ   4
#define BW_250KHZ   5
#define BW_500KHZ   6

#define BUFF_SIZE 64 // 受信用バッファサイズ(\r\n\0含む)

/* ---------------
 * 初期設定
 -----------------*/
// ピン設定
const int LoRa_power = 17; // LoRaのSleepと接続するPIN番号(任意:D2)
const int LoRa_Rst = 14; // LoRaのResetと接続するPIN番号(任意:D3)

class ES920LR2{
public:
  ES920LR2();
  void begin();
  void set_config(int in_bw, int in_sf);
  int LoRa_send(char *msg);

  //後で消す
  void debug();

private:
  char buf[BUFF_SIZE];
  int LoRa_recv(char *buf);
  void LoRa_reset();
  bool sendcmd(char *cmd);
};

#endif // ES920LR2_H
