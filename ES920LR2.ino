#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "AE_GPS.h"
#include "ES920LR2.h"

Adafruit_BMP085 bmp;
AE_GPS gps; // RX, TX
ES920LR2 lora;

int stage = 0;
float p0 = 0; // 射点気圧[Pa]
float p1 = 0;
bool flag = true;

void setup() {
  Serial.begin(115200);
  gps.begin();

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while(true){delay(1000);}
  }

  //ステージを待機中に設定
  stage=0;

  //ステージをデバッグ用に設定
  //stage=3;
}

void loop() {
  switch(stage){
    case 0:
      // 待機中
      stage_standby();
      break;
    case 1:
      // 飛行中
      stage_flight();
      break;
    case 2:
      // 降下中
      stage_descent();
      break;
    case 3:
      // デバッグ用
      if(flag){ //デバッグ用の初期化
        debug_start();
        flag=false;
      }
      stage_debug(); // デバッグ時の繰り返し関数
      break;
  }
  delay(1);
}

// 待機中の動作
void stage_standby(){
  float p = bmp.readPressure();

  // 射点気圧をp0に設定
  if(p0<1){
    p0=p;
  }else if(p0-p > 100){
    //p0から100Pa (約10m)気圧が下がったらステージを飛行中に変更
    stage=1;
  }
}

// 飛行中の動作
void stage_flight(){
  float p = bmp.readPressure();
  p1=p;
}

// 降下中の動作
void stage_descent(){
  
}


//----------------------------------------
// デバッグ用（初期化と接続テスト）
void debug_start(){
  Serial.println("debug_start");
  int res=2;
  Serial.println("test start");


  // GPSテスト
  gps.refresh();
  Serial.print("緯度:");
  Serial.print(gps.latitude);
  Serial.print(gps.NS);
  Serial.print(" 経度:");
  Serial.print(gps.longitude);
  Serial.println(gps.EW);

  // 気圧計テスト
  Serial.print(bmp.readTemperature());
  Serial.print(" *C, ");

  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  // LoRa 初期化と設定
  lora.begin();
  lora.set_config(BW_125KHZ, 9);
  lora.LoRa_send("send test\r\n");
}

// デバッグ用（GPSデータをLoRaで送信）
void stage_debug(){
  char str[24];

  // 緯度経度の文字列を連結
  strcpy(str, gps.latitude);
  strcat(str, ",");
  strcat(str, gps.longitude);
  strcat(str, "\r\n");

  // 緯度経度情報を送信
  lora.LoRa_send(str);

  // GPS情報を更新
  gps.refresh();

}
