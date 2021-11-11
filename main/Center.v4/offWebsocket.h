#ifndef offWebsocket_h
#define offWebsocket_h
#include <BlynkSimpleEsp8266.h>
#include <WebSocketsServer.h>
#include<EEPROM.h>

extern WebSocketsServer webSocket;
extern unsigned long previousOneMinuteCounter;
unsigned long previousCheckConnectOfCenterVsRelay = 0;

extern bool BLOCK, relayConnection;
//#define MQ3Pin D0 
//#define clearEEPROMPin D7
//#define blockNowPin D8

extern bool isAlcoholStatus; //thay 3 file
extern bool clearEEPROMStatus;
extern bool blockNowStatus;

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
    digitalWrite(2, LOW);
    //ledRelayState.on();
    //Blynk.virtualWrite(V1, 1);  
    previousCheckConnectOfCenterVsRelay = millis();

  }
  else if(payloadString == "relayOpening")
  {
    digitalWrite(2,HIGH);
    //ledRelayState.off();
    //Blynk.virtualWrite(V1, 0); 
    previousCheckConnectOfCenterVsRelay = millis();
  }
  else if(payloadString == "/")
  {
    if (EEPROM.read(100) == 0 && EEPROM.read(101) == 0)
    {
      clearEEPROMStatus = 1;
      digitalWrite(2,HIGH);
      BLOCK = 0;
    }
    else 
    {
      blockNowStatus = 1;
      digitalWrite(2,LOW);
      BLOCK = 1;
    } 
    previousCheckConnectOfCenterVsRelay = millis();
    relayConnection = 1;
    Blynk.virtualWrite(V6, " ");
  }
}
void updateWebsocket()  //Hàm này dùng để gửi đi, nếu (cái gì đó) thì gửi đoạn broadcasetTXT(gì đó)
{
  if (isAlcoholStatus==1) //chan gắn cảm biến nồng độ cồn
  {
    webSocket.broadcastTXT("detectAlcohol");
    BLOCK = 1;
    EEPROM.write(100,1);    // Nhớ sửa ở off_blynk.cpp
    EEPROM.write(101,224);  // Nhớ sửa ở off_blynk.cpp
    EEPROM.commit();
    Serial.println("detect Alcohol");
    Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V3, "Phát hiện nồng độ cồn");
    Blynk.virtualWrite(V4, "Mở khoá sau: 8 giờ 0 phút");      // Sửa ở đây// Sửa ở đây// Sửa ở đây// Sửa ở đây
    //ledRelayState.on();
    isAlcoholStatus = 0;
    previousOneMinuteCounter = millis();
  }
  if (clearEEPROMStatus==1) 
  {
    webSocket.broadcastTXT("clearEEPROM");
    BLOCK = 0;
    EEPROM.write(100,0);
    EEPROM.write(101,0);
    EEPROM.commit();
    Serial.println("Da xoa EEPROM");
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V3, "Không phát hiện nồng độ cồn");
    Blynk.virtualWrite(V4, "Khoá đã mở");
    //ledRelayState.off();
    clearEEPROMStatus = 0;
  }
  if (blockNowStatus==1)
  {
    webSocket.broadcastTXT("blockNow");
    BLOCK = 1;
    EEPROM.write(100,1);    // Nhớ sửa ở off_blynk.cpp
    EEPROM.write(101,224);  // Nhớ sửa ở off_blynk.cpp
    EEPROM.commit();
    Serial.println("Da Block Now");
    blockNowStatus = 0;
    previousOneMinuteCounter = millis();
  }
  if (millis() - previousCheckConnectOfCenterVsRelay > 25000) 
  {
    Blynk.virtualWrite(V6, "Lỗi kết nối");
    relayConnection = 0;
    previousCheckConnectOfCenterVsRelay = millis();
  }
}
#endif
