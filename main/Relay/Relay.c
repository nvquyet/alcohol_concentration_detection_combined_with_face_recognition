/* Arduino 1.8.13
 * ESP8266 Boards (2.5.0) 
 * 
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <WebSocketsClient.h>
#include <stdio.h>

#define relayPin 0
int k1_timedown, k2_timedown ;
bool BLOCK = 0;
int gio, phut;
unsigned long timedownPhut;
WebSocketsClient webSocket;
unsigned long previousMillis = 0, previousMillisRelay = 0;
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
    EEPROM.write(100,1);
    EEPROM.write(101,224);
    EEPROM.commit();
    Serial.println("Dong relay");
    previousMillisRelay = millis();
  }
  if(payloadString == "clearEEPROM")
  {
    digitalWrite(relayPin,HIGH);
    BLOCK = 0;
    EEPROM.write(100,0);
    EEPROM.write(101,0);
    EEPROM.commit();
    Serial.println("Cleared EEPROM");
  }
  if(payloadString == "blockNow")
  {
    digitalWrite(relayPin,LOW);
    BLOCK = 1;
    EEPROM.write(100,1);
    EEPROM.write(101,224);
    EEPROM.commit();
    Serial.println("BLOCKED");
    previousMillisRelay = millis();
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
  
  EEPROM.begin(512);
  Serial.begin(115200);
  k1_timedown = EEPROM.read(100);
  k2_timedown = EEPROM.read(101);
  delay(10);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  webSocket.begin(ip_host, port);
  webSocket.onEvent(webSocketEvent);
  Serial.println("1");
  previousMillisRelay = millis();
}

void loop() {
  k1_timedown = EEPROM.read(100);
  k2_timedown = EEPROM.read(101);
  timedownPhut = k1_timedown*256+k2_timedown;
  gio = timedownPhut/60;
  phut = timedownPhut%60;
  webSocket.loop();
  if (millis() - previousMillis > 10000) // có thể dùng time của blynk
  {
    previousMillis = millis();
    String mygio = String(gio);
    String myphut = String(phut);
    String JSONtxt = "{\"GIO\": \""+mygio+"\", \"PHUT\": \""+myphut+"\"}";
    Serial.println(JSONtxt);
    updateWebsocket();
  }
  if (millis() - previousMillisRelay > 60000 && (EEPROM.read(100) != 0 || EEPROM.read(101) != 0) )
  {
    previousMillisRelay = millis();
    k2_timedown--;
    EEPROM.write(101,k2_timedown);
    EEPROM.commit();
    if (k2_timedown == 0 && k1_timedown != 0)
    {
      k2_timedown = 255;
      k1_timedown--;
      EEPROM.write(100,k1_timedown);
      EEPROM.commit();
    }
  }
  delay(100);
}
