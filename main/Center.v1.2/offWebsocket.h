#ifndef offWebsocket_h
#define offWebsocket_h
#include <BlynkSimpleEsp8266.h>
#include <WebSocketsServer.h>
#include<EEPROM.h>

extern WebSocketsServer webSocket;
extern unsigned long previousOneMinuteCounter;
unsigned long previousCheckEEPROM = 0;
unsigned long previousCheckConnectOfCenterVsRelay = 0;

extern bool BLOCK;
#define MQ3Pin D0 
#define clearEEPROMPin D7
#define blockNowPin D8

extern WidgetLED ledConnect; ///Khai bao linh kien đã thêm trong app Blynk, có nhiều loại WidgetLCD, WidgetGPS,...
extern WidgetLED ledRelayState;


void webSocketEvent(uint8_t num, WStype_t type,
                    uint8_t * payload,
                    size_t length) // Hàm này dùng để nhận payloadSring từ module khác, rồi so sánh thực hiện lệnh
{
  String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);
  String s = "";
  if(payloadString == "relayClosing")
  {
    digitalWrite(2,LOW);
    ledRelayState.on();
    Blynk.virtualWrite(V1, 1);
    previousCheckConnectOfCenterVsRelay = millis();

  }
  else if(payloadString == "relayOpening")
  {
    digitalWrite(2,HIGH);
    ledRelayState.off();
    EEPROM.write(100,0);
    EEPROM.write(101,0);
    EEPROM.commit();
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V4, "Khoá đã mở");
    previousCheckConnectOfCenterVsRelay = millis();
  }
  else if(payloadString == "/")
  {
     if (EEPROM.read(100) == 0 && EEPROM.read(101) == 0)
      {
        digitalWrite(clearEEPROMPin, 0);
        digitalWrite(2,HIGH);
        BLOCK = 0;
      }
    previousCheckConnectOfCenterVsRelay = millis();
    Blynk.virtualWrite(V4, "Đang kết nối lại");
  }
}
void updateWebsocket()  //Hàm này dùng để gửi đi, nếu (cái gì đó) thì gửi đoạn broadcasetTXT(gì đó)
{
  if (digitalRead(MQ3Pin)==0) //chan gắn cảm biến nồng độ cồn
  {
    webSocket.broadcastTXT("detectAlcohol");
    BLOCK = 1;
    EEPROM.write(100,1);    // Nhớ sửa ở off_blynk.cpp
    EEPROM.write(101,224);  // Nhớ sửa ở off_blynk.cpp
    EEPROM.commit();
    Serial.println("detect Alcohol");
    Blynk.virtualWrite(V1, 1);
    ledRelayState.on();
    previousOneMinuteCounter = millis();
  }
  if (digitalRead(clearEEPROMPin)==0) 
  {
    webSocket.broadcastTXT("clearEEPROM");
    BLOCK = 0;
    EEPROM.write(100,0);
    EEPROM.write(101,0);
    EEPROM.commit();
    Serial.println("Da xoa EEPROM");
    digitalWrite(clearEEPROMPin, 1);
  }
  if (digitalRead(blockNowPin)==0)
  {
    webSocket.broadcastTXT("blockNow");
    BLOCK = 1;
    EEPROM.write(100,1);    // Nhớ sửa ở off_blynk.cpp
    EEPROM.write(101,224);  // Nhớ sửa ở off_blynk.cpp
    EEPROM.commit();
    Serial.println("Da Block Now");
    digitalWrite(blockNowPin, 1);
    previousOneMinuteCounter = millis();
  }
//  if (EEPROM.read(100) == 0 && EEPROM.read(101) == 0)
//  {
//    BLOCK = 0;
//    //Blynk.virtualWrite(V1, 1);
//    if( millis() - previousCheckEEPROM > 60*1000)
//    {
//      previousCheckEEPROM = millis();
//      EEPROM.write(100,0);
//      EEPROM.write(101,0);
//      EEPROM.commit();
//      webSocket.broadcastTXT("clearEEPROM");
//      Serial.println("Da xoa EEPROM");
//      digitalWrite(clearEEPROMPin, 1);
//    }
//  }
  if (millis() - previousCheckConnectOfCenterVsRelay > 12000) 
  {
    Blynk.virtualWrite(V4, "Lỗi kết nối");
    previousCheckConnectOfCenterVsRelay = millis();
  }
}
#endif
