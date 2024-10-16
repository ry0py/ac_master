#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <PubSubClient.h> //MQTTクライアントライブラリ

#define PIN_SEND 4
#define PIN_BUTTON_TURN_ON 0
#define PIN_BUTTON_TURN_OFF 18

#define IR_SEND_PIN PIN_SEND
#include <IRremote.hpp>

#define MQTT_MAX_PACKET_SIZE 64 //MQTTメッセージの最大サイズを指定
#define ledPin 32 // テスト用
// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓変更するところ↓↓↓↓↓↓↓↓↓↓↓↓↓↓
// #define dataPin 14 // 赤外線のデータを送信するピン
const char* ssid = "XXXXX"; //wifiのSSID // 5Gとかついてるやつは多分つながらない
const char* password = "xxxxx"; //wifiのパスワード

char On[] = "";
char Off[] = "";
char Heat25deg[] = "";
char Cool27deg[] = "";
char Dehuhigh[] = "";

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓必要があれば変更するところ↓↓↓↓↓↓↓↓↓↓↓↓↓↓

const char *clientID = "ESP32";
const char* topic =  "AirMaster/AirMaster"; //チャネル名/リソース名
const char* host = "mqtt.beebotte.com";
enum ACOperation {
    ON, OFF  };
String username = "token_xxxx"; // beeboteで作成したトークン

WiFiClientSecure wifiClient;
PubSubClient client(host, 8883, wifiClient);
  
void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.mode(WIFI_STA);

  IrSender.begin();
  pinMode(PIN_BUTTON_TURN_ON, INPUT_PULLUP);
  pinMode(PIN_BUTTON_TURN_OFF, INPUT_PULLUP);
  
  // pinModeは適宜変更
  // pinMode(dataPin, OUTPUT_OPEN_DRAIN);
  // pinMode(ledPin,OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(WiFi.status());
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Disable SSL certificate verification
  wifiClient.setInsecure();
}

void loop() {
  //MQTTに接続
  if (!client.connected()) {
    client.connect(clientID, username.c_str(), NULL);

    if (client.connected()) {
      Serial.println("MQTT connected");
      client.setCallback(callback);
      
      client.subscribe(topic);
    } else {
      Serial.print("MQTT connection failed: ");
      Serial.println(client.state());
      delay(5000);
    }
  } else {
    client.loop();
  }
}

// Beebotteからメッセージを受け取ったときに呼び出される関数
void callback(char* topic, byte* payload, unsigned int length) {
  char buffer[MQTT_MAX_PACKET_SIZE];
  
  snprintf(buffer, sizeof(buffer), "%s", payload);   
  Serial.println("received:");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.println(buffer);

  // 受け取ったメッセージをデコード
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, buffer);

  if (error) {
    Serial.println("deserializeJson() failed");
    return;
  }

  const char* parsedPayload = doc["data"];

  if (parsedPayload != NULL) {
    Serial.print("payload: ");
    Serial.println(parsedPayload);
    if (strcmp(parsedPayload, "ac_On") == 0) {
      IrSender.sendPronto(On);
      Serial.print("on");
      delay(500);
    } else if (strcmp(parsedPayload, "ac_Off") == 0) {
      IrSender.sendPronto(Off);
      Serial.print("off");
      delay(500);
    } else if (strcmp(parsedPayload, "ac_HeaterOn") == 0) {
      IrSender.sendPronto(Heat25deg);
      Serial.print("Heater");
      delay(500);
    } else if (strcmp(parsedPayload, "ac_DehuOn") == 0) {
      IrSender.sendPronto(Dehuhigh);
      Serial.print("Dehumidifier");
      delay(500);
    } else if (strcmp(parsedPayload, "ac_CoolerOn") == 0) {
      IrSender.sendPronto(Cool27deg);
      Serial.print("Cooler");
      delay(500);
    }
  }
}

// 赤外線のデータを送信するための関数【要変更】
void irSend(ACOperation ac_ope){
  switch(ac_ope){
    case ON:
      digitalWrite(ledPin, HIGH);
      break;
    case OFF:
      digitalWrite(ledPin, LOW);
      break;
  }
}

// 以下デバッグ用の関数
// ESPが検出してるWiFiのSSIDを列挙する関数
void searchWiFiName(){
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
}