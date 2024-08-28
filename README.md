# ac_master
ベースは[これ](https://ediy-fan.com/beebotte/)を参考にしていく
あと[これ](https://note.com/khe00716/n/n8064e6484037)
## 概要
![画像1 (1)](https://hackmd.io/_uploads/B1cM3n_sR.png)


## Line Bot(りょうぴ)
- Line Botの作り方+GASとの連携
[参考](https://qiita.com/cog1t0/items/cc7779345a01192d8f01)
 - LineのUIを**on**と**off**のボタンを二つにしたい
 - UIの工夫は[ここ](https://qiita.com/cog1t0/items/cc7779345a01192d8f01)
 - リッチメニューの作成
![image](https://hackmd.io/_uploads/rk-83h_sC.png)

## Beebotte(りょうぴ)
- MQTTというHTTPと似てるやつでESP32と通信

## ESP32(つっつ)
- MQTT通信で送られてきたデータをsubscribeをする
- そのデータをもとに赤外線でエアコンのオン、オフ信号を送る
- 赤外線で送るデータは[ここ](https://asukiaaa.blogspot.com/2021/12/copy-and-send-ir-signal-by-esp32-arduino.html)を参考にすればいけそう

