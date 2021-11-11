#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "offWebserver.h"

char auth[] = "3q079w5483spMKPq3dpG-tIKlk5s3NxY";
BlynkTimer timer;
WidgetLED led1(V0);
WidgetBridge bridgeTemperatureAndHumidity(V11);
BLYNK_CONNECTED(){
  bridgeTemperatureAndHumidity.setAuthToken("rZm_3EGgb1ygB8jJYkDitU7fepU9j7Rm");
}

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


//================AP==============================
const char *ssid_ap = "NhietdoDoam";
const char *password_ap = "1234567890";
IPAddress ip_ap(192,168,4,1);
IPAddress gateway_ap(192,168,4,1);
IPAddress subnet_ap(255,255,255,0);
//================STA===========================
//char ssid[] = "Nha Tro Tat Tung 2.4Ghz";
//char pass[] = "0963659772";
//=================Webserver để setting mật khẩu Wifi nhà=============================
ESP8266WebServer server(80);
//======================================================================
String ten_wifi;
String mk_wifi;
char ten_wifi_blynk[32];
char mk_wifi_blynk[64];
bool wifiFlash = 0;
unsigned long previousMaSo = 0;
float t1 = 0, h1 = 0;
bool state = 0;


void setup()
{ 
  EEPROM.begin(512);

  pinMode(2, OUTPUT);
  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.println("Đọc tên wifi trên EEPROM");
  for (int i = 0; i < 32; ++i)
  {
    ten_wifi += char(EEPROM.read(i));
  }
  Serial.print("Tên wifi: ");
  Serial.println(ten_wifi);
  Serial.println();
  Serial.println("Đọc mk wifi trên EEPROM: ");
  for (int i = 32; i < 96; ++i)
  {
    mk_wifi += char(EEPROM.read(i));
  }
  Serial.print("Mật khẩu: ");
  Serial.println(mk_wifi);
  Serial.println("");
  WiFi.begin(ten_wifi, mk_wifi);
  Serial.println("1");

  strcpy(ten_wifi_blynk, ten_wifi.c_str()); //chuyển chuỗi thành char[]
  strcpy(mk_wifi_blynk, mk_wifi.c_str());

  if (testWiFi())
  {
    Serial.println("Kết nối thành công!! ");
    Serial.print("Địa chỉ IP_STA: ");
    Serial.println(WiFi.localIP());
    digitalWrite(2, HIGH);
    delay(500);
    wifiFlash = 1;
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    delay(500);
    for(int i = 0; i<7; i++)
    {
      digitalWrite(2, LOW);
      delay(50);
      digitalWrite(2, HIGH);
      delay(50);
    }
    Blynk.begin(auth, ten_wifi_blynk, mk_wifi_blynk);
    Serial.println("2");
    delay(100);
    WiFi.mode(WIFI_OFF);
  }
  else
  {
    Serial.println("Cấu hình lại điểm kết nối");
    digitalWrite(2, LOW);
    WiFi.mode(WIFI_AP);
    WiFi.disconnect();
    WiFi.softAP(ssid_ap,password_ap);
    WiFi.softAPConfig(ip_ap,gateway_ap,subnet_ap);
    Serial.print("Địa chỉ IP_AP của ESP:. ");
    Serial.println(WiFi.softAPIP());
    wifiFlash = 0;
    server.on("/", handleWebsite);
    server.on("/caidat", cai_dat);
    server.begin();//bắt đầu khởi động server
    delay(100);
  } 
}

void loop()
{
  if (wifiFlash == 0) 
  {
    server.handleClient();
    state = !state;
    digitalWrite(2, state);
    delay(100);
  }
  else 
  {
    t1 = float(random(-5,5))/10; 
    h1 = float(random(-2,2));
    delay(200);
    float t = dht.readTemperature() ; 
    float h = dht.readHumidity() ;
    delay(4800);
    WiFi.forceSleepWake();
    delay(1);
    Blynk.begin(auth, ten_wifi_blynk, mk_wifi_blynk);
    Blynk.run();
    timer.run();
    delay(1);
    if (isnan(h) || isnan(t)) 
    {
      Blynk.virtualWrite(V1, "Nhiệt ẩm kế bị lỗi");
      Blynk.virtualWrite(V21, 0);
      bridgeTemperatureAndHumidity.virtualWrite(V21, 0);
      Blynk.virtualWrite(V22, 0);
      bridgeTemperatureAndHumidity.virtualWrite(V22, 0);
    }
    else
    {
      Blynk.virtualWrite(V21, t);
      bridgeTemperatureAndHumidity.virtualWrite(V21, t);
      Blynk.virtualWrite(V22, h);
      bridgeTemperatureAndHumidity.virtualWrite(V22, h);
      Blynk.virtualWrite(V1, "   "); 
    }
    delay(30);
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
    delay(10*60*1000-9167);

  }
}
