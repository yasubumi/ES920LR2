#include "ES920LR2.h"

ES920LR2::ES920LR2()
{

}

void ES920LR2::begin()
{
  Serial.println("LoRa.begin()");
  for(int i=0;i<5;i++){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  // シリアル接続
  Serial1.begin(115200);
  delay(200);

  pinMode(LoRa_Rst, OUTPUT); // リセット用PINをOUTPUTに設定。
  digitalWrite(LoRa_Rst, HIGH);

  // Power ON
  pinMode(LoRa_power, OUTPUT); // スリープ割り込み用PINをOUTPUTに設定。
  digitalWrite(LoRa_power, HIGH); // スリープ割り込みピンをHIGHに設定
  delay(1000);

  // 無線からの情報を読み込み
  LoRa_reset();
  delay(1500);
  LoRa_recv(buf);
  Serial.print(buf);
  Serial.println("LoRa start");
}

int ES920LR2::LoRa_recv(char *buf){
  /*
   * LoRaから受信。戻り値は受信文字数。
   */
  char *start = buf;
  int icount = 0;
  int itimeout = 0;

  while(true){
    delay(1);
    while(Serial1.available() > 0){
      *buf++ = Serial1.read();
      if(icount>=2){
        if(*(buf-2) == '\r' && *(buf-1) == '\n'){
          *buf = '\0';
          return (buf-start);
        }
      }
      icount++;
      if(icount>=BUFF_SIZE-1){
        //Serial.println("受信文字数上限超過エラー LoRa_recv()");// 受信文字数上限超過エラー
        return -1;
      }
    }
    // timeout処理
    itimeout++;
    if(itimeout >10000){
      //Serial.println("LoRa_recve() timeout");
      return -2;
    }
  }
}

void ES920LR2::set_config(int in_bw, int in_sf){
  /*
   * LoRaの設定。帯域幅、拡散率、スリープモードを設定する
   */
  char buf[BUFF_SIZE];
  LoRa_send("config\r\n");
  delay(200);
  LoRa_reset();
  delay(1500);

  // LoRaから"Select Mode"を受信するまで待機
  while(true){
    LoRa_recv(buf);
    if(strstr(buf, "Mode")){
      Serial.print(buf);
      break;
    }
  }

  // Modeはプロセッサモード:2を選択
  sendcmd("2\r\n");

  
  // 帯域幅設定
  sprintf(buf, "bw %d\r\n", in_bw);
  sendcmd(buf);
  // 拡散率設定
  sprintf(buf, "sf %d\r\n", in_sf);
  sendcmd(buf);
  // 動作モードをオペレーションモードに設定
  sendcmd("q 2\r\n");
  // EEPROMに設定をセーブ
  sendcmd("w\r\n"); 

  LoRa_reset();
  Serial.println("LoRa module setting finished");
  delay(1000);
}

int ES920LR2::LoRa_send(char *msg){
  /*
   * LoRaにコマンドを送信
   */
  char *start = msg;
  while(*msg != '\0'){
    Serial1.write(*msg++);
  }
  return (msg - start);
}

bool ES920LR2::sendcmd(char *cmd){
  /*
   * LoRaにコマンドを送る
   */
  unsigned long t;
  char buf[BUFF_SIZE]; // コマンド送付後のデータ受信バッファ

  Serial.print(cmd); // デバッグプリント
  LoRa_send(cmd); // データ送信

  //受信チェック
  while(true){
    LoRa_recv(buf);
    if(strstr(buf, "OK")){
      Serial.print(buf); // デバッグプリント
      return true;
    }else if(strstr(buf, "NG")){
      Serial.print(buf); // デバッグプリント
      return false;
    }
  }
}


void ES920LR2::LoRa_reset(){
  /*
   * LoRaをリセットする
   */
   Serial.println("LoRa_reset()"); // デバッグプリント
   digitalWrite(LoRa_Rst, LOW);
   delay(100);
   digitalWrite(LoRa_Rst, HIGH);
}

void ES920LR2::debug(){
  // 通信のテスト
  Serial.println("send test");
  char ibuf[BUFF_SIZE];
  int res=0;

  LoRa_send("send test\r\n");

  while(true){
    LoRa_recv(ibuf);
    if(strstr(ibuf, "OK")){
      Serial.print(ibuf);
      break;
    } else if(strstr(ibuf, "NG")){
      Serial.print(ibuf);
      break;
    }
  }

  Serial.print("END");
}
