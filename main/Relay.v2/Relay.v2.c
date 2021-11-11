/* 
 * ESP8266 Boards (2.5.0) 
 * 
*/

#include <WebSocketsClient.h>
#include <stdio.h>

#define relayPin 0
bool BLOCK = 1;
WebSocketsClient webSocket;
unsigned long previousMillis = 0;
const char* ssid = "CENTER_ESP8266"; //Đổi thành wifi của bạn
const char* password = "1234567890"; //Đổi pass luôn
const char* ip_host = "192.168.4.1"; //Đổi luôn IP host của PC nha
const uint16_t port = 81; //Port thích đổi thì phải đổi ở server nữa

void webSocketEvent(WStype_t type, 
                    uint8_t * payload, 
                    size_t length) 
{
    String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);
  String s = "";
  if(payloadString == "detectAlcohol")
  {
    digitalWrite(relayPin,LOW);
    BLOCK = 1;
    Serial.println("Dong relay");
  }
  if(payloadString == "clearEEPROM")
  {
    digitalWrite(relayPin,HIGH);
    BLOCK = 0;
    Serial.println("Cleared EEPROM");
  }
  if(payloadString == "blockNow")
  {
    digitalWrite(relayPin,LOW);
    BLOCK = 1;
    Serial.println("BLOCKED");
  }
  if(payloadString == "")
  {
    Serial.println("Lỗi");
    digitalWrite(2, LOW);
    delay(100);
    digitalWrite(2, HIGH);
    delay(50);
  }
}


void updateWebsocket()
{
  if (digitalRead(relayPin)==0)
  {
    webSocket.sendTXT("relayClosing");
  }
  else
  {
    webSocket.sendTXT("relayOpening");
  }
}

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  delay(10);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 4, 10), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  webSocket.begin(ip_host, port);
  webSocket.onEvent(webSocketEvent);
  Serial.println("1");
 
}

void loop() {
  webSocket.loop();
  if (millis() - previousMillis > 10000) // có thể dùng time của blynk
  {
    previousMillis = millis();
    updateWebsocket();
    Serial.println(WiFi.localIP());
  }
  delay(100);
}
