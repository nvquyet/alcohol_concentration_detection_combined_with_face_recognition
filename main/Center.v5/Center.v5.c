/* 
 * ESP8266 Boards (2.5.0) 
 * Thiết lập WIFI thủ công né 192.168.4.10 ra 
*/

#include <ESP8266WiFi.h>
//#include <pgmspace.h>
//#include <WebSocketsServer.h>
//#include <ESP8266WebServer.h>
//#include <EEPROM.h>
//#include <BlynkSimpleEsp8266.h>

#include "offBlynk.h"
#include "offWebserver.h"
#include "offWebsocket.h"
#include "offMeasureAlcoholConcentration.h"
//====================================OLED=================================
//#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>       //1.10.2
//#include <Adafruit_SSD1306.h>  //2.4.7
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3C for 128x64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };
//======================================================================
#define buttonPin D3
#define camTXPin D5
#define camRXPin D6
//================AP==============================
const char *ssid_ap = "CENTER_ESP8266";
const char *password_ap = "1234567890";
IPAddress ip_ap(192,168,4,1);
IPAddress gateway_ap(192,168,4,1);
IPAddress subnet_ap(255,255,255,0);

WidgetLED ledConnect(V0);
WidgetLED ledRelayState(V2);
WidgetBridge bridge1(V10);
BLYNK_CONNECTED(){
  bridge1.setAuthToken("Hgj-gX-zZcKNd4Llmiisb7wo9ALOHXPq");
}
//================STA===============================
//const char *ssid_sta = "Nha Tro Tat Tung 2.4Ghz";
//const char *password_sta = "0963659772";
//=========================Blynk================================
#define BLYNK_TEMPLATE_ID "TMPLHMDJTjMv"
#define BLYNK_DEVICE_NAME "TMPLHMDJTjMv"
//#define BLYNK_AUTH_TOKEN            "2TgJDneqXNqgQTw3Rb203hKEeApIQjmy"      //APP
#define BLYNK_AUTH_TOKEN            "rZm_3EGgb1ygB8jJYkDitU7fepU9j7Rm"     // chua xuat app
char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;
//=================Webserver để setting mật khẩu Wifi nhà=============================
ESP8266WebServer server(80);
//==============================================================================
WebSocketsServer webSocket = WebSocketsServer(81);
//===================================================================
bool wifiFlash = 0, relayConnection = 1, alcoholFlash = 0, wifiOnSSD1306CheckState = 0;
bool BLOCK = 1;
unsigned long previousMillis = 0, previousOneMinuteCounter = 0, previousUpdateStatusOnBlynk = 0;
int temperatureValue = 0;
int humidityValue = 0;

int timedownMinute;
int gio;
int phut;
String mygio;
String myphut;
String JSONtxt;

bool isAlcoholState = 0; //thay 3 file
bool clearEEPROMState = 0;
bool blockNowState = 0;
//==================================================================================================


void setup()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(200);
  
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), measureAlcoholConcentration, FALLING); //ngắt ko dùng được cho chân D0 (GPIO16)    //FALLING thì setup INPUT_PULLUP.
  pinMode(2, OUTPUT); //D4
  pinMode(camTXPin, OUTPUT);
  digitalWrite(camTXPin, 1);
  pinMode(camRXPin, INPUT_PULLUP);
  
  
  EEPROM.begin(512);
  Serial.begin(115200);

  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  String ten_wifi;
  String mk_wifi;
  char ten_wifi_blynk[] = "";
  char mk_wifi_blynk[] = "";
  Serial.println();
  Serial.println("Đọc tên wifi trên EEPROM");
  for (int i = 0; i < 32; ++i)
  {
    ten_wifi += char(EEPROM.read(i));
    
  }
  Serial.print("Tên wifi: ");
  Serial.println(ten_wifi);
  for (int i =0; i < 32; ++i) 
  {
    ten_wifi_blynk[i] = ten_wifi[i];
  }
  Serial.println();
  Serial.println("Đọc mk wifi trên EEPROM: ");
  for (int i = 32; i < 96; ++i)
  {
    mk_wifi += char(EEPROM.read(i));
  }
  Serial.print("Mật khẩu: ");
  Serial.println(mk_wifi);
  for (int i =0; i < 32; ++i) 
  {
    mk_wifi_blynk[i] = mk_wifi[i];
  }
  Serial.println("");
  Serial.println(ten_wifi_blynk);
  Serial.println(mk_wifi_blynk);
  WiFi.begin(ten_wifi, mk_wifi);
  delay(10);
  Serial.println("1");

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
    timer.setInterval(700L, blinkLedConnect);
    //timer.setInterval(1000L, isAlcohol);
    digitalWrite(2, 1); // ban dau cho tat led
    //ledRelayState.off();  
    WiFi.softAPConfig(ip_ap,gateway_ap,subnet_ap);
    WiFi.softAP(ssid_ap,password_ap);
    Serial.print("Địa chỉ IP_AP của ESP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", handleWebsite);
    server.on("/caidat", cai_dat);
    server.begin();//bắt đầu khởi động server
    delay(500);
  }
  else
  {
    Serial.println("Cấu hình lại điểm kết nối");       
    digitalWrite(2, LOW);
    WiFi.softAPConfig(ip_ap,gateway_ap,subnet_ap);
    WiFi.softAP(ssid_ap,password_ap);
    Serial.print("Địa chỉ IP_AP của ESP:. ");
    Serial.println(WiFi.softAPIP());
    wifiFlash = 0;
    server.on("/", handleWebsite);
    server.on("/caidat", cai_dat);
    server.begin();//bắt đầu khởi động server
    delay(500);
  }  
  EEPROM.write(100,1);    // Nhớ sửa ở off_blynk.cpp
  EEPROM.write(101,224);  // Nhớ sửa ở off_blynk.cpp
  EEPROM.commit();
  delay(1);
  BLOCK = 1;
  Blynk.virtualWrite(V1, 1);
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  previousOneMinuteCounter = millis();
  alcoholFlash = 0;
}
//===================================================================================================================================
void loop()
{ 
  if (wifiFlash == 0) //No wifi
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip_ap, gateway_ap, subnet_ap);
    WiFi.softAP(ssid_ap, password_ap);
    server.handleClient();
    webSocket.loop();
    int k1_timedown = EEPROM.read(100);
    int k2_timedown = EEPROM.read(101);
    if (millis() - previousMillis > 1000)   // có thể dùng time của blynk
    {
      previousMillis = millis();
      //Serial.print(".");
      updateWebsocket();
    }
    if (millis() - previousUpdateStatusOnBlynk > 1000)
    {
      previousUpdateStatusOnBlynk = millis();
      timedownMinute = k1_timedown*256+k2_timedown;
      gio = timedownMinute/60;
      phut = timedownMinute%60;
      mygio = String(gio);
      myphut = String(phut);
      JSONtxt = "{\"GIO\": \""+mygio+"\", \"PHUT\": \""+myphut+"\"}";
      if(BLOCK == 1)
      {
        Blynk.virtualWrite(V4, "Mở khoá sau: "+mygio+" giờ "+myphut+" phút");
        display.clearDisplay();
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(31,25);
        display.println(F("MO KHOA SAU:"));
        display.setCursor(25,37);
        display.print(mygio); display.print(" GIO "); display.print(myphut); display.print(" PHUT."); 
      }
      else
      {
        Blynk.virtualWrite(V4, "Khoá đã mở");
        display.clearDisplay();
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(37,20);
        display.println(F("DA MO KHOA"));
        display.setCursor(7,32);
        display.print("CHUC THUONG LO BINH           AN!");
      }
      display.setTextColor(WHITE);        // Draw white text
      if (relayConnection == 0)
      {
        display.setCursor(110,57);
        display.println(F("!RL"));             
      }
      else
      {
        display.setCursor(110,57);
        display.println(F("   "));
      }
      display.setCursor(0,0);
      if (wifiOnSSD1306CheckState == 0)
      {
        display.print("NO WIFI");
      }
      else
      {
        display.print("       ");
      }
      wifiOnSSD1306CheckState = !wifiOnSSD1306CheckState;
      display.setCursor(76,0);
      display.print(temperatureValue); display.print("`C");
      display.setCursor(110,0);
      display.print(humidityValue); display.print("%");
      display.display();
      delay(10);
    }
    if (millis() - previousOneMinuteCounter > 59604 && (EEPROM.read(100) != 0 || EEPROM.read(101) != 0) ) // 1 phút
    {
      Serial.println(JSONtxt);
      previousOneMinuteCounter = millis();
      k2_timedown--;
      EEPROM.write(101,k2_timedown);
      EEPROM.commit();
      if (k2_timedown == 0 && k1_timedown != 0)
      {
        k2_timedown = 255;
        k1_timedown--;
        EEPROM.write(101,k2_timedown);
        EEPROM.write(100,k1_timedown);
        EEPROM.commit();
      }
      else if(k2_timedown == 0 && k1_timedown == 0)
      {
        clearEEPROMState = 1;
        digitalWrite(2,HIGH);
        BLOCK = 0;
      }
    }
    if(alcoholFlash == 1)
      {
        display.clearDisplay();
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(10,20);
        display.println(F("THOI MANH  VAO ONG"));
        display.display();
        delay(200);
        readIsAlcohol();
        display.clearDisplay();
        display.display();
        delay(20);
      }
    
    Blynk.run();
    timer.run();
    delay(100);
  }
//========================================================================================
  else 
  {
    if (!check_connect()) 
    {
      Serial.println("Reconnect");
      digitalWrite(2, LOW);
      delay(100);
      ESP.reset();
    }
    else 
    { //viet code vao day//viet code vao day//viet code vao day//viet code vao day//viet code vao day//viet code vao day
      server.handleClient();
      webSocket.loop();
      int k1_timedown = EEPROM.read(100);
      int k2_timedown = EEPROM.read(101);
      if (millis() - previousMillis > 1000)   // có thể dùng time của blynk
      {
        previousMillis = millis();
        //Serial.print(".");
        updateWebsocket();
      }
      if (millis() - previousUpdateStatusOnBlynk > 1000)
      {
        previousUpdateStatusOnBlynk = millis();
        timedownMinute = k1_timedown*256+k2_timedown;
        gio = timedownMinute/60;
        phut = timedownMinute%60;
        mygio = String(gio);
        myphut = String(phut);
        JSONtxt = "{\"GIO\": \""+mygio+"\", \"PHUT\": \""+myphut+"\"}";
        if(BLOCK == 1)
        {
          Blynk.virtualWrite(V4, "Mở khoá sau: "+mygio+" giờ "+myphut+" phút");
          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(WHITE);        // Draw white text
          display.setCursor(31,25);
          display.println(F("MO KHOA SAU:"));
          display.setCursor(25,37);
          display.print(mygio); display.print(" GIO "); display.print(myphut); display.print(" PHUT."); 
        }
        else
        {
          Blynk.virtualWrite(V4, "Khoá đã mở");
          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(WHITE);        // Draw white text
          display.setCursor(37,20);
          display.println(F("DA MO KHOA"));
          display.setCursor(7,32);
          display.print("CHUC THUONG LO BINH           AN!");
        } 
        display.setTextColor(WHITE);        // Draw white text
        if (relayConnection == 0)
        {
          display.setCursor(110,57);
          display.println(F("!RL"));             
        }
        else
        {
          display.setCursor(110,57);
          display.println(F("   "));
        }
        display.setCursor(0,0);
        if (wifiOnSSD1306CheckState == 0)
        {
          display.print(" WIFI");
        }
        else
        {
          display.print("     ");
        }
        wifiOnSSD1306CheckState = !wifiOnSSD1306CheckState;
        display.setCursor(76,0);
        display.print(temperatureValue); display.print("`C");
        display.setCursor(110,0);
        display.print(humidityValue); display.print("%");
        display.display();
        //delay(1);
      }
      if (millis() - previousOneMinuteCounter > 59604 && (EEPROM.read(100) != 0 || EEPROM.read(101) != 0) ) // 1 phút
      {
        Serial.println(JSONtxt);
        previousOneMinuteCounter = millis();
        k2_timedown--;
        EEPROM.write(101,k2_timedown);
        EEPROM.commit();
        if (k2_timedown == 0 && k1_timedown != 0)
        {
          k2_timedown = 255;
          k1_timedown--;
          EEPROM.write(101,k2_timedown);
          EEPROM.write(100,k1_timedown);
          EEPROM.commit();
        }
        else if(k2_timedown == 0 && k1_timedown == 0)
        {
          clearEEPROMState = 1;
          digitalWrite(2,HIGH);
          BLOCK = 0;
        }
      }
      if(alcoholFlash == 1)
      {
        display.clearDisplay();
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(10,20);
        display.println(F("THOI MANH  VAO ONG"));
        display.display();
        delay(200);
        readIsAlcohol();
        display.clearDisplay();
        display.display();
        delay(20);
      }
      Blynk.run();
      timer.run();
      delay(100);
    }
  }
}
