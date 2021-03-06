/* 
 * ESP8266 Boards (2.5.0) 
 * 
*/

#include <ESP8266WiFi.h>
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
#define MQ3Pin D0 //thay 3 file
#define clearEEPROMPin D7
#define blockNowPin D8

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
//=================Webserver ????? setting m???t kh???u Wifi nh??=============================
ESP8266WebServer server(80);
//==============================================================================
WebSocketsServer webSocket = WebSocketsServer(81);
//===================================================================
bool wifiFlash = 0, state = 0, takePhotosState = 0 ;
bool BLOCK = 1, buttonFlash = 0;
unsigned long previousMillis = 0, previousOneMinuteCounter = 0, previousUpdateStatusOnBlynk = 0;
int temperatureValue = 0;
int humidityValue = 0;
/*
unsigned long previousCheckEEPROM = 0;
//==============================================
//==============================================
//==============================================
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
    Blynk.virtualWrite(V3, "Ph??t hi???n n???ng ????? c???n");    //ghi gi?? tr??? l??n bi???n ???o c?? th??? xu???t ???????c trong App
  }
  else
  {
    Blynk.virtualWrite(V3, "Kh??ng ph??t hi???n n???ng ????? c???n");
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
    Blynk.virtualWrite(V4, "M??? kho?? sau: 8 gi??? 0 ph??t");      // S???a ??? ????y// S???a ??? ????y// S???a ??? ????y// S???a ??? ????y
    Serial.println("{\"GIO\": \"8\", \"PHUT\": \"0\"}");  // S???a ??? ????y// S???a ??? ????y// S???a ??? ????y// S???a ??? ????y
    ledRelayState.on();
    digitalWrite(blockNowPin, 0);
    
  }
  else
  {
    Blynk.virtualWrite(V1, 0);
    ledRelayState.off();
    digitalWrite(clearEEPROMPin, 0);
  }
}
//==============================================
//==============================================
//==============================================
#include <pgmspace.h>
const char PAGE_NetworkConfiguration[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title >ESP k???t n???i WIFI</title>
</head>
<body>
  <h1 style ="color:red; font-size: 40px;">Getting the password</h1>
  <form action="caidat">
    <label style ="color:blue; font-size:30px ;" for="tenWiFi"> T??n WiFi:</label> 
    <input style ="font-size: 20px;"  type="text" id="tenWiFi"  name="tenWiFi"> <br><br>
    <label style="color: blue; font-size: 30px;" for="matkhau">M???t kh???u:</label>
    <input style ="font-size: 20px;"  type="text" id="matkhau"  name="matkhau"> <br><br>
    <input style="color: green; font-size: 20px; " type="submit">
  </form>
</body>
</html>
)=====";
// name la phan quan trong nhat
void handleWebsite(void) {
  server.send(200, "text/html", PAGE_NetworkConfiguration); //g???i d?????i d???ng html
}

// Ham cai dat de luu ten va mat khau wifi vao ROM 
void cai_dat(void) {
  String tenwf = server.arg("tenWiFi");
  String mk = server.arg("matkhau");
  Serial.print("ten wifi:");
  Serial.println(tenwf);
  Serial.print("mat khau:");
  Serial.println(mk);
  // Kiem tra ten va mk doc duoc co hay khong 
  if (tenwf.length() > 0 && mk.length() > 0) {
    Serial.println("clear EEPROM");
    // write a 0 to 96 bytes of the EEPROM
    for (int i = 0; i < 96; ++i) {
      EEPROM.write(i, 0);
    }
    delay(200);
    Serial.println("Ch??p t??n WiFi v??o EEPROM:");
    for (int i = 0; i < tenwf.length(); ++i) {
      EEPROM.write(i, tenwf[i]);
      Serial.print("vi???t t??n: ");
      Serial.println(tenwf[i]);
    }
    Serial.println("Ch??p m???t kh???u WiFi v??o EEPROM:");
    for (int i = 0; i < mk.length(); ++i) {
      EEPROM.write(32 + i, mk[i]);
      Serial.print("vi???t mk: ");
      Serial.println(mk[i]);
    }
    delay(200);
    Serial.println("Commit");
    EEPROM.commit();
    delay(100);
    Serial.println("Reset");
  }
  // Sau khi luu xong reset chuong trinh 
  ESP.reset();
}

// Ham test ket noi wifi luc dau 
bool testWiFi(void)
{
  int c = 0;
  Serial.println("Wait to connect");
  while (c < 20) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println();
  Serial.println("Kh??ng th??? k???t n???i v?? qu?? th???i gian ch??? ");
  return false;
}

// check connect to wifi 
bool check_connect(void)
{
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  else {
    return false;
  }
}

//==============================================
//==============================================
//==============================================
void webSocketEvent(uint8_t num, WStype_t type,
                    uint8_t * payload,
                    size_t length) // H??m n??y d??ng ????? nh???n payloadSring t??? module kh??c, r???i so s??nh th???c hi???n l???nh
{
  String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);
  String s = "";
  if(payloadString == "relayDong")
  {
    digitalWrite(2,LOW);
    ledRelayState.on();
    Blynk.virtualWrite(V1, 1);

  }
  else if(payloadString == "relayMo")
  {
    digitalWrite(2,HIGH);
    ledRelayState.off();
    EEPROM.write(100,0);
    EEPROM.write(101,0);
    EEPROM.commit();
    Blynk.virtualWrite(V1, 0);
  }
  else 
  {
    digitalWrite(2,HIGH);
    ledRelayState.off();
    Blynk.virtualWrite(V1, 0);
  }
}
void updateWebsocket()  //H??m n??y d??ng ????? g???i ??i, n???u (c??i g?? ????) th?? g???i ??o???n broadcasetTXT(g?? ????)
{
  if (digitalRead(MQ3Pin)==0) //chan g???n c???m bi???n n???ng ????? c???n
  {
    webSocket.broadcastTXT("phatHienNongDoCon");
    BLOCK = 1;
    EEPROM.write(100,1);    // Nh??? s???a ??? off_blynk.cpp
    EEPROM.write(101,164);  // Nh??? s???a ??? off_blynk.cpp
    EEPROM.commit();
    Serial.println("Phat hien nong do con");
    Blynk.virtualWrite(V1, 1);
    ledRelayState.on();
    previousOneMinuteCounter = millis();
  }
  if (digitalRead(clearEEPROMPin)==0) 
  {
    BLOCK = 0;
    EEPROM.write(100,0);
    EEPROM.write(101,0);
    EEPROM.commit();
    webSocket.broadcastTXT("clearEEPROM");
    Serial.println("Da xoa EEPROM");
    digitalWrite(clearEEPROMPin, 1);
  }
  if (digitalRead(blockNowPin)==0)
  {
    BLOCK = 1;
    EEPROM.write(100,1);    // Nh??? s???a ??? off_blynk.cpp
    EEPROM.write(101,164);  // Nh??? s???a ??? off_blynk.cpp
    EEPROM.commit();
    webSocket.broadcastTXT("blockNow");
    Serial.println("Da Block Now");
    digitalWrite(blockNowPin, 1);
    previousOneMinuteCounter = millis();
  }
  if (EEPROM.read(100) == 0 && EEPROM.read(101) == 0)
  {
    BLOCK = 0;
    //Blynk.virtualWrite(V1, 1);
    if( millis() - previousCheckEEPROM > 10000)
    {
      previousCheckEEPROM = millis();
      EEPROM.write(100,0);
      EEPROM.write(101,0);
      EEPROM.commit();
      webSocket.broadcastTXT("clearEEPROM");
      Serial.println("Da xoa EEPROM");
      digitalWrite(clearEEPROMPin, 1);
    }
  }
}
//==============================================
//==============================================
//==============================================*/


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
  
  pinMode(MQ3Pin,INPUT_PULLUP); //D0 Nong ????? c???n
  pinMode(clearEEPROMPin, OUTPUT); // clearEEPROM
  digitalWrite(clearEEPROMPin, 1);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(blockNowPin, OUTPUT); // blockNow
  digitalWrite(blockNowPin, 1);
  pinMode(2,OUTPUT); //D4
  pinMode(camTXPin, OUTPUT);
  digitalWrite(camTXPin,1);
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
  Serial.println("?????c t??n wifi tr??n EEPROM");
  for (int i = 0; i < 32; ++i)
  {
    ten_wifi += char(EEPROM.read(i));
    
  }
  Serial.print("T??n wifi: ");
  Serial.println(ten_wifi);
  for (int i =0; i < 32; ++i) 
  {
    ten_wifi_blynk[i] = ten_wifi[i];
  }
  Serial.println();
  Serial.println("?????c mk wifi tr??n EEPROM: ");
  for (int i = 32; i < 96; ++i)
  {
    mk_wifi += char(EEPROM.read(i));
  }
  Serial.print("M???t kh???u: ");
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
    Serial.println("K???t n???i th??nh c??ng!! ");
    Serial.print("?????a ch??? IP_STA: ");
    Serial.println(WiFi.localIP());
    digitalWrite(2, HIGH);
    delay(500);
    wifiFlash = 1;
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    delay(500);
    digitalWrite(2, LOW);
    delay(50);
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    delay(50);
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    delay(50);
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    delay(50);
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    delay(50);
    digitalWrite(2, HIGH);
    delay(50);

    
    Serial.println("2");
    timer.setInterval(300L, blinkLedConnect);
    timer.setInterval(1000L, isAlcohol);
    digitalWrite(2, 1); // ban dau cho tat led
    ledRelayState.off();
    if (EEPROM.read(100) != 0 || EEPROM.read(101) != 0)
    {
      BLOCK = 1;
      Blynk.virtualWrite(V1, 1);
    }
    else
    {
      Blynk.virtualWrite(V1, 0);
    }
    
    WiFi.softAPConfig(ip_ap,gateway_ap,subnet_ap);
    WiFi.softAP(WiFi.localIP().toString()+"_"+(String)ssid_ap,password_ap);
    Serial.print("?????a ch??? IP_AP c???a ESP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", handleWebsite);
    server.on("/caidat", cai_dat);
    server.begin();//b???t ?????u kh???i ?????ng server
    delay(500);
  }
  else
  {
    Serial.println("C???u h??nh l???i ??i???m k???t n???i");
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0,5);
    display.println(F("Cau hinh lai diem ket noi"));
    display.println(F("Dang nhap 192.168.4.1"));
    display.display();
    delay(100);
       
    digitalWrite(2, LOW);
    WiFi.softAPConfig(ip_ap,gateway_ap,subnet_ap);
    WiFi.softAP(ssid_ap,password_ap);
    Serial.print("?????a ch??? IP_AP c???a ESP:. ");
    Serial.println(WiFi.softAPIP());
    wifiFlash = 0;
    server.on("/", handleWebsite);
    server.on("/caidat", cai_dat);
    server.begin();//b???t ?????u kh???i ?????ng server
    delay(500);
  }  

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  previousOneMinuteCounter = millis();
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
//      if (EEPROM.read(100) == 0 && EEPROM.read(101) == 0 && millis() - previousOneMinuteCounter > 20000 && BLOCK == 1)
//      {
//        digitalWrite(clearEEPROMPin, 0);
//        digitalWrite(2,HIGH);
//        BLOCK = 0;
//      }
      int k1_timedown = EEPROM.read(100);
      int k2_timedown = EEPROM.read(101);
      if (millis() - previousMillis > 1000)   // c?? th??? d??ng time c???a blynk
      {
        previousMillis = millis();
        //Serial.print(".");
        updateWebsocket();
      }
      if (millis() - previousOneMinuteCounter > 59550 && (EEPROM.read(100) != 0 || EEPROM.read(101) != 0) ) // 1 ph??t
      {
        previousOneMinuteCounter = millis();
        k2_timedown--;
        EEPROM.write(101,k2_timedown);
        EEPROM.commit();
        if (k2_timedown == 0 && k1_timedown != 0)
        {
          k2_timedown = 255;
          k1_timedown--;
          EEPROM.write(100,k1_timedown);
          EEPROM.commit();
        }
        else if(k2_timedown == 0 && k1_timedown == 0)
        {
          digitalWrite(clearEEPROMPin, 0);
          digitalWrite(2,HIGH);
          BLOCK = 0;
        }
      }
      if (millis() - previousUpdateStatusOnBlynk > 10000)
      {
        previousUpdateStatusOnBlynk = millis();
        int timedownMinute = k1_timedown*256+k2_timedown;
        int gio = timedownMinute/60;
        int phut = timedownMinute%60;
        String mygio = String(gio);
        String myphut = String(phut);
        String JSONtxt = "{\"GIO\": \""+mygio+"\", \"PHUT\": \""+myphut+"\"}";
        Serial.println(JSONtxt);
        if(BLOCK == 1)
        {
          Blynk.virtualWrite(V4, "M??? kho?? sau: "+mygio+" gi??? "+myphut+" ph??t");
          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(WHITE);        // Draw white text
          display.setCursor(0,1);
          display.println(F("Mo khoa sau:"));
          display.print(mygio); display.print(" gio "); display.print(myphut); display.print(" phut."); 
        }
        else
        {
          Blynk.virtualWrite(V4, "Kho?? ???? m???");
          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(WHITE);        // Draw white text
          display.setCursor(0,1);
          display.println(F("Khoa da mo"));
        } 
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(76,57);
        display.print(temperatureValue); display.print("`C");
        display.setCursor(110,57);
        display.print(humidityValue); display.print("%");
        display.display();
        delay(10);
      }
      Blynk.run();
      timer.run();
      if(readMeasureAlcoholConcentrationButton() == true)
      {
        buttonFlash = 1;
        digitalWrite(camTXPin, 0);
        Serial.println("readMeasureAlcoholConcentrationButton() == true");
      }
      if (buttonFlash == 1)
      {
        readIsAlcohol();
      }
    }
  }

 delay(10);
}
