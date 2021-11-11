#ifndef offWebsocket_h
#define offWebsocket_h
#include <BlynkSimpleEsp8266.h>
#include <WebSocketsServer.h>
#include<EEPROM.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>       //1.10.2
#include <Adafruit_SSD1306.h>  //2.4.7
extern Adafruit_SSD1306 display;

extern WebSocketsServer webSocket;
extern unsigned long previousOneMinuteCounter;
unsigned long previousCheckConnectOfCenterVsRelay = 0;

extern bool BLOCK, relayConnection, wifiFlash;

extern bool isAlcoholState; //thay 3 file
extern bool notAlcoholState;
extern bool blockNowState;
extern bool unblockNowState;
extern int timedownMinute;
extern float milligramsPerLiter;

extern WidgetLED ledConnect; ///Khai bao linh kien đã thêm trong app Blynk, có nhiều loại WidgetLCD, WidgetGPS,...
extern WidgetLED ledRelayState;
bool k = 0;

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
    previousCheckConnectOfCenterVsRelay = millis();

  }
  else if(payloadString == "relayOpening")
  {
    digitalWrite(2,HIGH); 
    previousCheckConnectOfCenterVsRelay = millis();
  }
  else if(payloadString == "/")
  {
    if (timedownMinute == 0)
    {
      webSocket.broadcastTXT("blockNow");
    }
    else 
    {
      webSocket.broadcastTXT("unblockNow");
    } 
    previousCheckConnectOfCenterVsRelay = millis();
    relayConnection = 1;
    delay(10);
    Blynk.virtualWrite(V6, " ");
    delay(20);
    k = 1;
  }
}
void updateWebsocket()  //Hàm này dùng để gửi đi, nếu (cái gì đó) thì gửi đoạn broadcasetTXT(gì đó)
{
  if (isAlcoholState==1) //chan gắn cảm biến nồng độ cồn
  {
    webSocket.broadcastTXT("detectAlcohol");
    timedownMinute = 0;
    BLOCK = 1;
    Serial.println("detect Alcohol");
    isAlcoholState = 0;
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(12,8);
    display.println(F("PHAT HIEN"));
    display.setCursor(18,28);
    display.println(F("NONG DO"));
    display.setCursor(45,48);
    display.println(F("CON"));
    display.display();
    if(wifiFlash == 1)
    {
      updateToTable();
      delay(10);
      Blynk.virtualWrite(V1, 1);
      delay(20);
      if(milligramsPerLiter <= 2)
      {
        String v3 = "Nồng độ cồn: " + String(milligramsPerLiter, 3) + " mg/L";
        Blynk.virtualWrite(V3, v3);
      }
      else
      {
        Blynk.virtualWrite(V3, "Nồng độ cồn: > 2 mg/L");
      }
      
      delay(20);
      Blynk.notify("Tài xế đang có nồng độ cồn!");
      delay(20);
      Blynk.email ("Nồng độ cồn", "Tài xế đang có nồng độ cồn");
      delay(20);
    }
    delay(2000);
  }
//======
  if (notAlcoholState==1) 
  {
    webSocket.broadcastTXT("notDetectAlcohol");
    int k1_timedown = EEPROM.read(100);
    int k2_timedown = EEPROM.read(101);
    timedownMinute = k1_timedown*256+k2_timedown;
    BLOCK = 0;
    Serial.println("Đã mở khoá");
    notAlcoholState = 0;
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(3,8);
    display.println(F("KHONG PHAT"));
    display.setCursor(12,28);
    display.println(F("HIEN NONG"));
    display.setCursor(27,48);
    display.println(F("DO CON"));
    display.display();
    if(wifiFlash == 1)
    {
      delay(10);
      Blynk.virtualWrite(V1, 0);
      delay(20);
      Blynk.virtualWrite(V3, "Không phát hiện nồng độ cồn");
      delay(20);
    }
    delay(2000);
    previousOneMinuteCounter = millis();
  }
//======
  if (blockNowState==1)
  {
    webSocket.broadcastTXT("blockNow");
    timedownMinute = 0;
    BLOCK = 1;
    Serial.println("Đã Block Now");
    blockNowState = 0;
  }
//=======
  if (unblockNowState==1)
  {
    webSocket.broadcastTXT("unblockNow");
    int k1_timedown = EEPROM.read(100);
    int k2_timedown = EEPROM.read(101);
    timedownMinute = k1_timedown*256+k2_timedown;
    BLOCK = 0;
    Serial.println("Đã Unblock Now");
    unblockNowState = 0;
    previousOneMinuteCounter = millis();
  }
//======

  if (millis() - previousCheckConnectOfCenterVsRelay > 25000 && k == 1) 
  {
    delay(50);
    Blynk.virtualWrite(V6, "   Lỗi kết nối với Relay");
    Serial.println("      Lỗi kết nối với Relay");
    unsigned long t = millis();
    while(millis() - t < 1000)
    ESP.reset();
  }
}
#endif
