#include <Arduino.h>
#include <EEPROM.h>
void setup()
{
  EEPROM.begin(512);
  EEPROM.write(105,1);    // Nhớ sửa ở off_blynk.cpp
    // Nhớ sửa ở off_blynk.cpp
  EEPROM.commit();
}
//  String str = "   ";
//
//  for (int i = 0; i < 96; ++i) {
//    EEPROM.write(i, 0);
//  }
//  delay(200);
//  for (int i = 0; i < str.length(); ++i) {
//    EEPROM.write(i, str[i]);
//    Serial.print("viết tên: ");
//    Serial.println(str[i]);
//  }
//  delay(200);
//  Serial.println("Commit");
//  EEPROM.commit();
//}



void loop() 
{
  
}
