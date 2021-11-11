#ifndef offBlynk_h
#define offBlynk_h
#include <BlynkSimpleEsp8266.h>
#include<EEPROM.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

extern WidgetRTC rtc;
extern BlynkTimer timer;
extern int rowIndex;
String currentDateTime;

extern WidgetLED ledConnect; ///Khai bao linh kien đã thêm trong app Blynk, có nhiều loại WidgetLCD, WidgetGPS,...
extern WidgetLED ledRelayState;

extern bool BLOCK;
extern int temperatureValue;
extern int humidityValue;

extern bool isAlcoholState; //thay 3 file
extern bool notAlcoholState;
extern bool blockNowState;
extern bool unblockNowState;

extern int downLimit, upLimit, threshold;


void updateToTable() 
{
  if(rowIndex >= 21)
  {
    rowIndex = 1;
  }
  delay(10);
  Blynk.virtualWrite(V10, "add", rowIndex, currentDateTime, "Tài xế say");
  delay(20);
  //highlighting latest added row in table
  Blynk.virtualWrite(V10, "pick", rowIndex);
  delay(20);
  rowIndex++;
  EEPROM.write(106,rowIndex);
  EEPROM.commit();
}

void clockCurrent()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

  String currentTime = String(hour()) + ":" + minute();
  String currentDate = String(day()) + "-" + month() + "-" + year();
  currentDateTime = currentDate + "   " + currentTime;
}

// V0 LED Widget is blinking
void blinkLedConnect()
{
  if (ledConnect.getValue()) 
  {
    ledConnect.off();
    delay(20);
  } 
  else 
  {
    ledConnect.on();
    delay(20);
  }
}

BLYNK_WRITE(V1) //blockNow
{
  // Set incoming value from pin V1 to a variable
  //int value = param.asInt();
  if (BLOCK == 1)
  {
    delay(10);
    Blynk.virtualWrite(V1, 0);
    delay(20);
    Blynk.virtualWrite(V4, "Kiểm tra nồng độ cồn sau: 6 giờ 0 phút");      // Sửa ở đây// Sửa ở đây// Sửa ở đây// Sửa ở đây
    delay(20);
    unblockNowState = 1;
    
  }
  else
  {
    delay(10);
    Blynk.virtualWrite(V1, 1);
    delay(20);
    Blynk.virtualWrite(V4, "   Đã khoá");
    delay(20);
    blockNowState = 1;
  }
}
BLYNK_WRITE(V11)
{
  if (param.asFloat() == 1)
  {
    rowIndex = 1;
    EEPROM.write(105,rowIndex);
    delay(10);
    Blynk.virtualWrite(V10, "clr");
    delay(20);
  }
}

BLYNK_WRITE(V21)
{
  //if (param.asFloat() != 0)
  {
    delay(10);
    temperatureValue = param.asFloat();
    delay(10);
  }
}
BLYNK_WRITE(V22)
{
  //if (param.asFloat() != 0)
  {
    delay(10);
    humidityValue = param.asFloat();
    delay(10);
  }
}

BLYNK_WRITE(V30)
{
    delay(10);
    EEPROM.write(100, param.asFloat()/256);
    EEPROM.write(101, param.asInt()%256);
    EEPROM.commit();
    Serial.print("update 100th & 101st ROM: ");
    Serial.print((EEPROM.read(100)*256+EEPROM.read(101))/60); Serial.print(" Giờ "); Serial.print((EEPROM.read(100)*256+EEPROM.read(101))%60); Serial.println(" Phút");
    delay(10);
}

BLYNK_WRITE(V32)
{
    delay(10);
    EEPROM.write(102, param.asFloat());
    EEPROM.commit();
    delay(10);
    downLimit = EEPROM.read(102);
    Serial.print("update downLimit = ");
    Serial.println(downLimit);
    delay(10);
}

BLYNK_WRITE(V33)
{
    delay(10);
    EEPROM.write(103, param.asFloat());
    EEPROM.commit();
    delay(10);
    upLimit = EEPROM.read(103);
    Serial.print("update upLimit = ");
    Serial.println(upLimit);   
    delay(10);
}

BLYNK_WRITE(V34)
{
    delay(10);
    EEPROM.write(104, param.asFloat()/256);
    EEPROM.write(105, param.asInt()%256);
    EEPROM.commit();
    delay(10);
    threshold = EEPROM.read(104)*256+EEPROM.read(105);
    Serial.print("update threshold = ");
    Serial.println(threshold);
    delay(10);
}


#endif
