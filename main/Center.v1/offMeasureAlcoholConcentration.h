extern bool buttonFlash;
#define buttonPin D3
extern WidgetBridge bridge1;
unsigned long t1 , t2;
int MAX, MIN;
bool readMeasureAlcoholConcentrationButton()
{
  //bool measureAlcoholConcentrationState = 1;
  if (digitalRead(buttonPin) == LOW)
  {
    delay(50); //chống rung phím
    if(digitalRead(buttonPin) == HIGH)
    {
      delay(50);
      t1 = millis();
      t2 = millis();
      MAX = 0;
      MIN = 1024; 
      return true;
    }
    delay(50);
    return false;
  }
  delay(50);
  return false;
}


int readIsAlcohol()
{ 
  if (millis() - t1 < 15000)
  {
    if (millis() - t2 > 500)
    {
      Serial.print(":::::");
      t2 = millis();
      int current = analogRead(A0);
      if (current > MAX)
      {
        MAX = current;
      }
      if (current < MIN)
      {
        MIN = current;
      }
    }
  }
  else
  {
    t1 = millis();
    buttonFlash = 0;
    bridge1.digitalWrite(14, 0);
    Serial.print("MAX: ");
    Serial.println(MAX);
    Serial.print("MIN: ");
    Serial.println(MIN);
    if (480 - MIN > 50)
    {
      Serial.println("Làm ơn thổi lại");
    }
    else if (MAX - 480 > 100)
    {
      Serial.println("Phát hiện nồng độ cồn");
    }
    else
    {
      Serial.println("Không phát hiện nồng độ cồn");
    }
  }
}
