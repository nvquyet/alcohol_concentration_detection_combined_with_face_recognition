extern bool isAlcoholStatus;
bool isMatchFaceID = 0;
#define buttonPin D3
#define camTXPin D5
#define camRXPin D6

ICACHE_RAM_ATTR void measureAlcoholConcentration()  ///co the ko can ICACHE_RAM_ATTR 
{
    unsigned long t = millis();
    while (millis()-t<100)
    digitalWrite(camTXPin, 0);
    int MAX = 0;
    int MIN = 1024; 
    t = millis();
    unsigned long t_laymau = millis();
    while (millis()-t < 5000)
    {
      if (millis() - t_laymau > 500)
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
      }
    }
    Serial.print("MAX: ");
    Serial.println(MAX);
    Serial.print("MIN: ");
    Serial.println(MIN);
    if(isMatchFaceID == 1)
    {
      if (MAX - 480 > 100)
      {
        Serial.println("Phát hiện nồng độ cồn");
        isAlcoholStatus = 1;
      }
      else if (480 - MIN < 50)
      {
        Serial.println("Làm ơn thổi mạnh hơn");
      }
      else
      {
        Serial.println("Không phát hiện nồng độ cồn");
        clearEEPROMStatus = 1;
      }
    }
    else
    {
      Serial.println("Không nhận diện được tài xế");
    } 
    digitalWrite(camTXPin, 1);
    isMatchFaceID = 0;
}
