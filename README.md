# ES920LR2をArduinoから操作するスケッチ例

LoRaモジュールのES920LR2をArduinoから操作するスケッチ

ピンの前提などはArduino Nano Everyで実装

動作確認が取れたら必要な機能だけライブラリ化したい

## 参考URL
https://ambidata.io/samples/network/lora-2/

## 注意
Resetと繋ぐPINはオープンコレクタ(ドレイン)推奨になってるがArduino Nano Everyだとオープンドレイン使えないので、トランジスタ外付けして使用する予定。
このためPINのHIGH/LOWとOutputのHIGH/LOWが逆転する。
