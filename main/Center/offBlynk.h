#ifndef offBlynk_h
#define offBlynk_h
#include <BlynkSimpleEsp8266.h>
#define MQ3Pin 0 // D3
#define clearEEPROMPin D1
#define blockNowPin D2

extern WidgetLED ledConnect; ///Khai bao linh kien đã thêm trong app Blynk, có nhiều loại WidgetLCD, WidgetGPS,...
extern WidgetLED ledRelayState;

extern bool BLOCK;

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

void isAlcohol()
{
  if (digitalRead(MQ3Pin)==0)
  {
    Blynk.virtualWrite(V3, "Phát hiện nồng độ cồn");    //ghi giá trị lên biến ảo có thể xuất được trong App
    Blynk.virtualWrite(V4, "Mở khoá sau: 8 giờ 0 phút");// Sửa ở đây// Sửa ở đây// Sửa ở đây// Sửa ở đây
  }
  else
  {
    Blynk.virtualWrite(V3, "Không phát hiện nồng độ cồn");
  }
}

// This function is called every time the Virtual Pin 1 state changes // Dung cho button
BLYNK_WRITE(V1) //blockNow
{
  // Set incoming value from pin V1 to a variable
  //int value = param.asInt();
  if (BLOCK == 0)
  {
    Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V4, "Mở khoá sau: 8 giờ 0 phút");      // Sửa ở đây// Sửa ở đây// Sửa ở đây// Sửa ở đây
    //Serial.println("{\"GIO\": \"8\", \"PHUT\": \"0\"}");  // Sửa ở đây// Sửa ở đây// Sửa ở đây// Sửa ở đây
    ledRelayState.on();
    digitalWrite(blockNowPin, 0);
    
  }
  else
  {
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V4, "Khoá đã mở");
    ledRelayState.off();
    digitalWrite(clearEEPROMPin, 0);
  }
}
#endif
