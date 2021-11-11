/* Arduino 1.8.13
 * ESP8266 Boards (2.5.0) 
 * Thiết lập WIFI thủ công né 192.168.4.10 ra 
*/

/*
 * Dùng với Relay v.3 trở đi
 * CENTER:  lúc reset thì timedownMinute = 0, BLOCK = 1 (tức gửi lệnh cho relay đóng), lúc này, cần nhấn nút để kiểm tra nồng độ cồn,
            nếu có cồn thì ko thay đổi, nếu không có cồn thì đọc 1 lần duy nhất tổng 2 ô ram xong ghi vào timedownMinute.
  
 * RELAY:   lúc reset luôn đóng, sau đó kiểm tra tình trạng từ CENTER, nếu thổi không có nồng độ cồn
            thì mở, còn lại thì đóng. Sau đó CENTER đếm ngược timedownMinute trở về 0 thì gửi lệnh đóng RELAY.

 * CAMERA:  nhận diện khuôn mặt đăng kí trước, gửi kiểm tra tới CENTER
  
 * Nhiệt độ độ ẩm:     gửi nhiệt độ, độ ẩm mỗi 10 phút.



 * Lúc CENTER hoặc RELAY khởi động        =>  payloadString == "/"    =>  if (payloadString == "/" && timedownMinute == 0) gửi 1 lần duy nhất webSocket.broadcastTXT("blockNow");
                                                                          if (payloadString == "/" && timedownMinute != 0) gửi 1 lần duy nhất webSocket.broadcastTXT("readEEPROM");
 * Lúc CENTER đếm ngược timedownMinute    =>  blockNowState   = 1     =>  gửi 1 lần duy nhất webSocket.broadcastTXT("blockNow");                              BLOCK = 1; blockNowState = 0;                                                                          
 * Có nồng độ cồn                         =>  isAlcoholState  = 1     =>  gửi 1 lần duy nhất webSocket.broadcastTXT("detectAlcohol"); set timedownMinute = 0; BLOCK = 1; isAlcoholState = 0;                                                                          
 * Không có nồng độ cồn                   =>  notAlcoholState = 1     =>  gửi 1 lần duy nhất webSocket.broadcastTXT("notDetectAlcohol"); đọc 1 lần duy nhất tổng 2 ô ram xong ghi vào timedownMinute; BLOCK =0; notAlcoholState = 0;
 * Khoá bằng app                          =>  blockNowState   = 1     =>  gửi 1 lần duy nhất webSocket.broadcastTXT("blockNow");      set timedownMinute = 0; BLOCK = 1, blockNowState = 0; 
 * Mở bằng app                            =>  unblockNowState = 1     =>  gửi 1 lần duy nhất webSocket.broadcastTXT("unblockNow"); đọc 1 lần duy nhất tổng 2 ô ram xong ghi vào timedownMinute; BLOCK =0; unblockNowState = 0;
    
 */
#include <Arduino.h>
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
WidgetRTC rtc;
//WidgetBridge bridge1(V10);
//BLYNK_CONNECTED(){
//  bridge1.setAuthToken("Hgj-gX-zZcKNd4Llmiisb7wo9ALOHXPq");
//}
//================STA===============================
//const char *ssid_sta = "Nha Tro Tat Tung 2.4Ghz";
//const char *password_sta = "0963659772";
//=========================Blynk================================
#define BLYNK_TEMPLATE_ID "TMPLHMDJTjMv"
#define BLYNK_DEVICE_NAME "TMPLHMDJTjMv"
#define BLYNK_AUTH_TOKEN            "rZm_3EGgb1ygB8jJYkDitU7fepU9j7Rm"     // chua xuat app
char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;
//=================Webserver để setting mật khẩu Wifi nhà=============================
ESP8266WebServer server(80);
//==============================================================================
WebSocketsServer webSocket = WebSocketsServer(81);
//===================================================================
bool wifiFlash = 0, relayConnection = 0, alcoholFlash = 0, wifiOnSSD1306CheckState = 0;
bool BLOCK = 1;
unsigned long previousMillis = 0, previousOneMinuteCounter = 0, previousUpdateStatusOnBlynk = 0;
int temperatureValue = 0;
int humidityValue = 0;

int timedownMinute = 0;
int gio;
int phut;
String mygio;
String myphut;
String JSONtxt;

bool isAlcoholState = 0; //thay 3 file
bool notAlcoholState = 0;
bool blockNowState = 0;
bool unblockNowState = 0;

int downLimit, upLimit, threshold;
int rowIndex;
//==================================================================================================


void setup()
{
  EEPROM.begin(512);
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), measureAlcoholConcentration, FALLING); //ngắt ko dùng được cho chân D0 (GPIO16)    //FALLING thì setup INPUT_PULLUP.
  pinMode(2, OUTPUT); //D4
  pinMode(camTXPin, OUTPUT);
  digitalWrite(camTXPin, 1);
  pinMode(camRXPin, INPUT_PULLUP);
  
  downLimit = EEPROM.read(102);
  upLimit = EEPROM.read(103);
  threshold = EEPROM.read(104)*256+EEPROM.read(105);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(200);
  
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
      delay(20);
      digitalWrite(2, HIGH);
      delay(20);
    }
    Blynk.begin(auth, ten_wifi_blynk, mk_wifi_blynk);
    delay(20);
    Serial.println("2");
    timer.setInterval(1000L, blinkLedConnect);
    timer.setInterval(57000L, clockCurrent);
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
  delay(1);
  rtc.begin();
  rowIndex = EEPROM.read(106);
  BLOCK = 1;
  delay(10);
  Blynk.virtualWrite(V1, 1);
  delay(20);
  Blynk.virtualWrite(V6, "   Lỗi kết nối với Relay");
  delay(20);
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
    if (millis() - previousMillis > 1000)   // có thể dùng time của blynk
    {
      previousMillis = millis();
      //Serial.print(".");
      updateWebsocket();
    }
  //====
    
    if (millis() - previousUpdateStatusOnBlynk > 1000)
    {
      previousUpdateStatusOnBlynk = millis();
      gio = timedownMinute/60;
      phut = timedownMinute%60;
      mygio = String(gio);
      myphut = String(phut);
      JSONtxt = "{\"GIO\": \""+mygio+"\", \"PHUT\": \""+myphut+"\"}";
      if(BLOCK == 0)
      {
        //Blynk.virtualWrite(V4, "Khoá sau: "+mygio+" giờ "+myphut+" phút");
        display.clearDisplay();
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(37,17);
        display.println(F("DA MO KHOA"));
        display.setCursor(4,28);
        display.print(F("KT SAU:")); display.print(mygio); display.print(" GIO "); display.print(myphut); display.print(" PHUT");
        display.setCursor(7,39);
        display.print("CHUC THUONG LO BINH           AN!");
      }
      else
      {
        //Blynk.virtualWrite(V4, "   Đã khoá");
        display.clearDisplay();
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(46,19);
        display.println(F("DA KHOA"));
        display.setCursor(14,32);
        display.print("VUI LONG KIEM TRA");
        display.setCursor(30,43);
        display.print("NONG DO CON!");
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
  //====
    
    if (millis() - previousOneMinuteCounter > 59953 && (timedownMinute != 0) ) // 1 phút
    {      
      previousOneMinuteCounter = millis();
      timedownMinute--;
      if(timedownMinute == 0)
      {
        blockNowState = 1;
        digitalWrite(2,LOW);
        BLOCK = 1;
      }
      Serial.print("Phải kiểm tra nồng độ cồn sau: ");
      Serial.println(JSONtxt);
    }
    
  //====
  
    if(alcoholFlash == 1)
    {
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(WHITE);        // Draw white text
      display.setCursor(10,20);
      display.println(F("THOI MANH  VAO ONG"));
      display.display();
      delay(200);
      readIsAlcohol(downLimit, upLimit, threshold);
      display.clearDisplay();
      display.display();
      delay(20);;
    }
    
    Blynk.run();
    timer.run();
    delay(10);
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
      if (millis() - previousMillis > 1000)   // có thể dùng time của blynk
      {
        previousMillis = millis();
        //Serial.print(".");
        updateWebsocket();
      }
    //====
      
      if (millis() - previousUpdateStatusOnBlynk > 1000)
      {
        previousUpdateStatusOnBlynk = millis();
        gio = timedownMinute/60;
        phut = timedownMinute%60;
        mygio = String(gio);
        myphut = String(phut);
        JSONtxt = "{\"GIO\": \""+mygio+"\", \"PHUT\": \""+myphut+"\"}";
        if(BLOCK == 0)
        {
          delay(10);
          Blynk.virtualWrite(V4, "Khoá sau: "+mygio+" giờ "+myphut+" phút");
          delay(20);
          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(WHITE);        // Draw white text
          display.setCursor(37,17);
          display.println(F("DA MO KHOA"));
          display.setCursor(4,28);
          display.print(F("KT SAU:")); display.print(mygio); display.print(" GIO "); display.print(myphut); display.print(" PHUT");
          display.setCursor(7,39);
          display.print("CHUC THUONG LO BINH           AN!");
        }
        else
        {
          delay(10);
          Blynk.virtualWrite(V4, "   Đã khoá");
          delay(20);
          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(WHITE);        // Draw white text
          display.setCursor(46,19);
          display.println(F("DA KHOA"));
          display.setCursor(14,32);
          display.print("VUI LONG KIEM TRA");
          display.setCursor(30,43);
          display.print("NONG DO CON!");
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
        delay(10);
      }
    //====
      
      if (millis() - previousOneMinuteCounter > 59953 && (timedownMinute != 0) ) // 1 phút
      {
        previousOneMinuteCounter = millis();
        Serial.print("Phải kiểm tra nồng độ cồn sau: ");
        Serial.println(JSONtxt);
        timedownMinute--;
        if(timedownMinute == 0)
        {
          blockNowState = 1;
          digitalWrite(2,LOW);
          BLOCK = 1;
        }
      }
      
    //====
    
      if(alcoholFlash == 1)
      {
        display.clearDisplay();
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(10,20);
        display.println(F("THOI MANH  VAO ONG"));
        display.display();
        delay(200);
        readIsAlcohol(downLimit, upLimit, threshold);
        display.clearDisplay();
        display.display();
        delay(20);
      }
      
      Blynk.run();
      timer.run();
      delay(10);
    }
  }
}
