#ifndef offBlynk_h
#define offBlynk_h
#include <BlynkSimpleEsp8266.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>       //1.10.2
#include <Adafruit_SSD1306.h>  //2.4.7

//#define MQ3Pin D0 
//#define clearEEPROMPin D7
//#define blockNowPin D8

extern Adafruit_SSD1306 display;

extern WidgetLED ledConnect; ///Khai bao linh kien đã thêm trong app Blynk, có nhiều loại WidgetLCD, WidgetGPS,...
extern WidgetLED ledRelayState;

extern bool BLOCK;
extern int temperatureValue;
extern int humidityValue;

extern bool isAlcoholStatus; //thay 3 file
extern bool clearEEPROMStatus;
extern bool blockNowStatus;

// V0 LED Widget is blinking
void blinkLedConnect()
{
  if (ledConnect.getValue()) 
  {
    ledConnect.off();
  } 
  else 
  {
    ledConnect.on();
  }
}

//void isAlcohol()
//{
//  if (isAlcoholStatus==1)
//  {
//    Blynk.virtualWrite(V3, "Phát hiện nồng độ cồn");    //ghi giá trị lên biến ảo có thể xuất được trong App
//    Blynk.virtualWrite(V4, "Mở khoá sau: 8 giờ 0 phút");// Sửa ở đây// Sửa ở đây// Sửa ở đây// Sửa ở đây
//  }
//  else
//  {
//    Blynk.virtualWrite(V3, "Không phát hiện nồng độ cồn");
//  }
//}

// This function is called every time the Virtual Pin 1 state changes // Dung cho button
BLYNK_WRITE(V1) //blockNow
{
  // Set incoming value from pin V1 to a variable
  //int value = param.asInt();
  if (BLOCK == 0)
  {
    Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V4, "Mở khoá sau: 8 giờ 0 phút");      // Sửa ở đây// Sửa ở đây// Sửa ở đây// Sửa ở đây
    //ledRelayState.on();
    blockNowStatus = 1;
    
  }
  else
  {
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V4, "Khoá đã mở");
    //ledRelayState.off();
    clearEEPROMStatus = 1;
  }
}
BLYNK_WRITE(V21)
{
  if (param.asFloat() != 0)
  {
    temperatureValue = param.asFloat();
  }
}
BLYNK_WRITE(V22)
{
  if (param.asFloat() != 0)
  {
    humidityValue = param.asFloat();
  }
}


#endif
