/*
 * ES920LR2で通信するテスト
 * PinはArduino Nano Everyを前提に設定している。
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
#define BW_62.5KHZ  3
#define BW_125KHZ   4
#define BW_250KHZ   5
#define BW_500KHZ   6


/* ---------------
 * 初期設定
 -----------------*/
// ピン設定
const int LoRa_TX = 0; // LoRaのTXと接続するPIN番号 (RX:D0)
const int LoRa_RX = 1; // LoRaのRXと接続するPIN番号 (TX:D1)
const int LoRa_Sleep = 2; // LoRaのSleepと接続するPIN番号(任意:D2)
const int LoRa_Rst = 3; // LoRaのResetと接続するPIN番号(任意:D3)

// 無線設定
const int LoRa_BW = BW_125KHZ; // 帯域幅
const int LoRa_SF = 9; // 拡散率(5~12)
const int LoRa_Sleep_Mode = SLEEP_NO_SLEEP; // ひとまずスリープモードにはしない

const int recv_buf_size = 100; // 受信用バッファサイズ(\r\n\0含む)

/* ---------------
 * 自作関数
 -----------------*/
void OpenDrainLow(int in_pin){
  /*
   * オープンドレインLOW
   * PINは外付けトランジスタのベースへ接続。出力をコレクタと接続する。要プルアップ
   * PINのHIGH/LOWと出力のHIGH/LOWが逆転する。
   */
   digitalWrite(in_pin, HIGH);
}

void OpenDrainHiZ(int in_pin){
  /*
   * オープンドレインHIGH
   * LOWと同様
   */
  digitalWrite(in_pin, LOW);
}

int LoRa_recv(char *buf){
  /*
   * LoRaから受信。戻り値は受信文字数。
   */
  char *start = buf;
  int icount = 0;

  while(true){
    delay(0);
    while(Serial1.available() > 0){
      *buf++ = Serial.read();
      if(icount>=2){
        if(*(buf-2) == '\r' && *(buf-1) == '\n'){
          *buf = '\0';
          return (buf-start);
        }
      }
      icount++;
      if(icount>=recv_buf_size-1){
        Serial.println("受信文字数上限超過エラー LoRa_recv()");// 受信文字数上限超過エラー
        return 0;
      }
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
   OpenDrainLow(LoRa_Rst);
   delay(100);
   OpenDrainHiZ(LoRa_Rst);
   delay(1000);
}

void set_config(int in_bw, int in_sf, int in_sleep){
  /*
   * LoRaの設定。帯域幅、拡散率、スリープモードを設定する
   */
  char buf[recv_buf_size];
  LoRa_send("config\r\n");
  delay(200);
  LoRa_reset();

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
  Serial.println("LoRa module set to new mode");
  delay(500);
}
/*-----------------
 * メイン処理 
 ------------------*/
void setup() {
  /*
   * 初期化処理
   */

  Serial.begin(9600);
  delay(20);

  // 無線と接続する為の初期化。
  Serial1.begin(115200);
  delay(20);

  pinMode(LoRa_Rst, OUTPUT); // リセット用PINをOUTPUTに設定。

  LoRa_reset();
  // 無線からの情報を読み込み
  while(Serial1.available()>0){
    char c = Serial1.read();
    if(c < 0x80){
      Serial.print(c);
    }
  }
  Serial.println("");
  Serial.println("Init Finished");

  // 無線設定
  set_config(LoRa_BW, LoRa_SF, LoRa_Sleep_Mode);

}

void loop() {
  // put your main code here, to run repeatedly:

}
