/*
 * ES920LR2で通信するテスト
 * PinはArduino Nano Everyを前提に設定。
 * 
 */

/* ---------------
 * 色々定義
 -----------------*/
// スリープモード
#define SLEEP_NO_SLEEP            1
#define SLLEP_TIMER_WAKEUP        2
#define SLEEP_INT_WAKEUP_CONTINUE 3
#define SLEEP_INT_WAKEUP_ONETIME  4
#define SLEEP_UART_WAKEUP         5

// 帯域幅
#define BW_62_5KHZ  3
#define BW_125KHZ   4
#define BW_250KHZ   5
#define BW_500KHZ   6


/* ---------------
 * 初期設定
 -----------------*/
// ピン設定
const int LoRa_TX = 0; // LoRaのTXと接続するPIN番号 (RX:D0)
const int LoRa_RX = 1; // LoRaのRXと接続するPIN番号 (TX:D1)
const int LoRa_power = 2; // LoRaのSleepと接続するPIN番号(任意:D2)
const int LoRa_Rst = 3; // LoRaのResetと接続するPIN番号(任意:D3)

// 無線設定
const int LoRa_BW = BW_125KHZ; // 帯域幅
const int LoRa_SF = 9; // 拡散率(5~12)
const int LoRa_Sleep_Mode = SLEEP_NO_SLEEP; // ひとまずスリープモードにはしない設定
//const int LoRa_Sleep_Mode = SLEEP_INT_WAKEUP_CONTINUE;

const int recv_buf_size = 64; // 受信用バッファサイズ(\r\n\0含む)

/* ---------------
 * プロトタイプ宣言 (無くても良いがライブラリ化に向けて準備)
 -----------------*/
void OpenDrainLow(int in_pin);
void OpenDrainHiZ(int in_pin);

int LoRa_recv(char *buf);
int LoRa_send(char *msg);
void LoRa_reset();

bool sendcmd(char *cmd);
void set_config(int in_bw, int in_sf, int in_sleep);


/* ---------------
 * 自作関数
 -----------------*/
int LoRa_recv(char *buf){
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
      if(icount>=recv_buf_size-1){
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

int LoRa_send(char *msg){
  /*
   * LoRaにコマンドを送信
   */
  char *start = msg;
  while(*msg != '\0'){
    Serial1.write(*msg++);
  }
  return (msg - start);
}

bool sendcmd(char *cmd){
  /*
   * LoRaにコマンドを送る
   */
  unsigned long t;
  char buf[recv_buf_size]; // コマンド送付後のデータ受信バッファ

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

void LoRa_reset(){
  /*
   * LoRaをリセットする
   */
   Serial.println("LoRa_reset()"); // デバッグプリント
   digitalWrite(LoRa_Rst, LOW);
   delay(100);
   digitalWrite(LoRa_Rst, HIGH);
}

void set_config(int in_bw, int in_sf, int in_sleep){
  /*
   * LoRaの設定。帯域幅、拡散率、スリープモードを設定する
   */
  char buf[recv_buf_size];
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
  // スリープモード設定
  sprintf(buf, "s %d\r\n", in_sleep);
  sendcmd(buf);
  // 動作モードをオペレーションモードに設定
  sendcmd("q 2\r\n");
  // EEPROMに設定をセーブ
  sendcmd("w\r\n"); 

  LoRa_reset();
  Serial.println("LoRa module setting finished");
  delay(500);

  // 通信のテスト
  Serial.println("send test");
  char ibuf[recv_buf_size];
  int res=0;
  sendcmd("start\r\n");
  LoRa_send("send test");

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

/*-----------------
 * メイン処理 
 ------------------*/
void setup() {
  /*
   * 初期化処理
   */
  char buf[recv_buf_size];
  Serial.begin(115200);
  
  pinMode(LoRa_power, OUTPUT); // スリープ割り込み用PINをOUTPUTに設定。
  digitalWrite(LoRa_power, HIGH); // スリープ割り込みピンをHIGHに設定

  // 無線と接続する為の初期化。
  Serial1.begin(115200);
  delay(1000);

  pinMode(LoRa_Rst, OUTPUT); // リセット用PINをOUTPUTに設定。
  digitalWrite(LoRa_Rst, HIGH);



  // 無線機リセット
  LoRa_reset();
  delay(1000);

  // 無線からの情報を読み込み
  LoRa_recv(buf);
  Serial.print(buf);

  Serial.println("");
  Serial.println("Init Finished");

  /* 起動した時のモードがコンフィグモードだった場合の処理をここに記載すること。
   * 具体的には、プロセッサモード選択後にコンフィグを実施してオペレーションモードへ移行する。
   * 起動時にオペレーションモードだった場合はコマンドが文字列として出力される。
   * この場合は最初に電波出るけど平気だろうか？何かしら対策入れておく必要あるかも。
   * 例えばTXモードで受信のみをデフォにしておいて、電波を出すときに切り替えるとか。
  */

  // 無線設定
  Serial.println("Setting Start");
  set_config(LoRa_BW, LoRa_SF, LoRa_Sleep_Mode);

}

void loop() {

}
