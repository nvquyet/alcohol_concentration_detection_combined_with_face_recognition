float Vcc = 5;
extern float RO = 241.919192;    //RO=((Vcc*RL)/Vout - RL)/60, while Vout = A/1024*3.3      A là giá trị analog đo được trung bình khi để ở không khí sạch. 
float RL = 1000;


//⁅((5120000/4.7𝑥−1000)/(5120000/(3.3⋅100)−1000))^(−1.43068)⋅0.4⋅〖60〗^(−1.43068)⁆

#include <BlynkSimpleEsp8266.h>
extern bool isAlcoholState, notAlcoholState, alcoholFlash;
bool isMatchFaceID = 0;
#define buttonPin D3
#define camTXPin D5
#define camRXPin D6

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>       //1.10.2
#include <Adafruit_SSD1306.h>  //2.4.7
extern Adafruit_SSD1306 display;

unsigned long t1 , t_laymau;
int MAX, MIN;
float milligramsPerLiter;

ICACHE_RAM_ATTR void measureAlcoholConcentration()  ///co the ko can ICACHE_RAM_ATTR 
{
  
    unsigned long t = millis();
    while (millis()-t<100)
    t1 = millis();
    t_laymau = millis();
    digitalWrite(camTXPin, 0);
    MAX = 0;
    MIN = 1024;
    alcoholFlash = 1;
} 

int readIsAlcohol(int downLimit, int upLimit,  int threshold)
{ 
  if (millis() - t1 < 7000)
  {
    if (millis() - t_laymau > 200)
    {
      Serial.println(":::::");
      t_laymau = millis();
      int current = analogRead(A0);
      if (current > MAX)
      {
        MAX = current;
      }
      if (current < MIN)
      {
        MIN = current;
      }
      if (digitalRead(camRXPin) == 0)
      {
        isMatchFaceID = 1;
      }
      milligramsPerLiter = 0.4 * pow((((Vcc*RL)/(MAX*4.7/1024)) - RL) / RO, -1.43068);
      Serial.println(RO);
      Serial.println(milligramsPerLiter,6);
    }
  }
  else
  {
    alcoholFlash = 0;
    Serial.print("MAX: ");
    Serial.println(MAX);
    Serial.print("MIN: ");
    Serial.println(MIN);
    String maxmin = "Max = " + String(MAX) + "| Min = " + String(MIN) + " ||| " +String(milligramsPerLiter,8);
    delay(10);
    Blynk.virtualWrite(V35, maxmin);
    delay(20);
    if(isMatchFaceID == 1)
    {
      if (MAX - threshold > upLimit)
      {
        Serial.println("Phát hiện nồng độ cồn");
        
        isAlcoholState = 1;
      }
      else if (threshold - MIN < downLimit)
      {
        Serial.println("Làm ơn thổi mạnh hơn");
        display.clearDisplay();
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(25,8);
        display.println(F("LAM ON"));
        display.setCursor(12,28);
        display.println(F("THOI MANH"));
        display.setCursor(45,48);
        display.println(F("HON"));
        display.display();
        delay(2000);
      }
      else
      {
        Serial.println("Không phát hiện nồng độ cồn");
        notAlcoholState = 1;
      }
    }
    else
    {
      Serial.println("Không nhận diện được tài xế");
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(WHITE);        // Draw white text
      display.setCursor(3,8);
      display.println(F("KHONG NHAN"));
      display.setCursor(12,28);
      display.println(F("DIEN DUOC"));
      display.setCursor(25,48);
      display.println(F("TAI XE"));
      display.display();
      delay(2000);
    } 
    digitalWrite(camTXPin, 1);
    isMatchFaceID = 0;
  }
}
