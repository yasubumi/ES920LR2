# ES920LR2をArduinoから操作するスケッチ例

LoRaモジュールのES920LR2をArduino Nano Everyから操作するスケッチ


## 参考URL
https://ambidata.io/samples/network/lora-2/

## 注意
GPSとES920LR2の2つをシリアル通信で使用するため、
Arduino Nano Everyの設定ファイルを修正してD3, D6をUART2として使えるようにする必要がある。
以下を参考に設定してください。
https://iqcompany.jp/2020/03/19/1508/
