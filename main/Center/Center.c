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

#define MQ3Pin 0 // D3
#define clearEEPROMPin D1
#define blockNowPin D2
//================AP==============================
const char *ssid_ap = "ESP8266";
const char *password_ap = "1234567890";
IPAddress ip_ap(192,168,4,1);
IPAddress gateway_ap(192,168,4,1);
IPAddress subnet_ap(255,255,255,0);

WidgetLED ledConnect(V0);
WidgetLED ledRelayState(V2);
//================STA===============================
//const char *ssid_sta = "Nha Tro Tat Tung 2.4Ghz";
//const char *password_sta = "0963659772";
//=========================Blynk================================
#define BLYNK_TEMPLATE_ID "TMPL8Sl1fHcn"
#define BLYNK_DEVICE_NAME "KHKT"
//#define BLYNK_AUTH_TOKEN            "ZsioMVx1ZONnldylPWTHVwCs0QKSe-6p"      //APP
#define BLYNK_AUTH_TOKEN            "rZm_3EGgb1ygB8jJYkDitU7fepU9j7Rm"     // chua xuat app
char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;
//=================Webserver để setting mật khẩu Wifi nhà=============================
ESP8266WebServer server(80);
//==============================================================================
WebSocketsServer webSocket = WebSocketsServer(81);
//===================================================================
bool wifiFlash = 0, state = 0;
bool BLOCK = 1, buttonFlash;
unsigned long previousMillis = 0, previousOneMinuteCounter = 0, previousUpdateStatusOnBlynk = 0;
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
    Blynk.virtualWrite(V3, "Phát hiện nồng độ cồn");    //ghi giá trị lên biến ảo có thể xuất được trong App
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
    Serial.println("{\"GIO\": \"8\", \"PHUT\": \"0\"}");  // Sửa ở đây// Sửa ở đây// Sửa ở đây// Sửa ở đây
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
  <title >ESP kết nối WIFI</title>
</head>
<body>
  <h1 style ="color:red; font-size: 40px;">Getting the password</h1>
  <form action="caidat">
    <label style ="color:blue; font-size:30px ;" for="tenWiFi"> Tên WiFi:</label> 
    <input style ="font-size: 20px;"  type="text" id="tenWiFi"  name="tenWiFi"> <br><br>
    <label style="color: blue; font-size: 30px;" for="matkhau">Mật khẩu:</label>
    <input style ="font-size: 20px;"  type="text" id="matkhau"  name="matkhau"> <br><br>
    <input style="color: green; font-size: 20px; " type="submit">
  </form>
</body>
</html>
)=====";
// name la phan quan trong nhat
void handleWebsite(void) {
  server.send(200, "text/html", PAGE_NetworkConfiguration); //gửi dưới dạng html
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
    Serial.println("Chép tên WiFi vào EEPROM:");
    for (int i = 0; i < tenwf.length(); ++i) {
      EEPROM.write(i, tenwf[i]);
      Serial.print("viết tên: ");
      Serial.println(tenwf[i]);
    }
    Serial.println("Chép mật khẩu WiFi vào EEPROM:");
    for (int i = 0; i < mk.length(); ++i) {
      EEPROM.write(32 + i, mk[i]);
      Serial.print("viết mk: ");
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
  Serial.println("Không thể kết nối vì quá thời gian chờ ");
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
                    size_t length) // Hàm này dùng để nhận payloadSring từ module khác, rồi so sánh thực hiện lệnh
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
void updateWebsocket()  //Hàm này dùng để gửi đi, nếu (cái gì đó) thì gửi đoạn broadcasetTXT(gì đó)
{
  if (digitalRead(MQ3Pin)==0) //chan gắn cảm biến nồng độ cồn
  {
    webSocket.broadcastTXT("phatHienNongDoCon");
    BLOCK = 1;
    EEPROM.write(100,1);    // Nhớ sửa ở off_blynk.cpp
    EEPROM.write(101,164);  // Nhớ sửa ở off_blynk.cpp
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
    EEPROM.write(100,1);    // Nhớ sửa ở off_blynk.cpp
    EEPROM.write(101,164);  // Nhớ sửa ở off_blynk.cpp
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
  pinMode(MQ3Pin,INPUT_PULLUP); //D3 Nong độ cồn
  pinMode(clearEEPROMPin, OUTPUT); // clearEEPROM
  pinMode(blockNowPin, OUTPUT); // blockNow
  pinMode(2,OUTPUT); //D4
  pinMode(D5, INPUT_PULLUP); //kiem tra nong do Button
  digitalWrite(clearEEPROMPin, 1);
  digitalWrite(blockNowPin, 1);
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

    Blynk.begin(auth, ten_wifi_blynk, mk_wifi_blynk);
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
      BLOCK = 0;
      Blynk.virtualWrite(V1, 0);
    }
    
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
      int k1_timedown = EEPROM.read(100);
      int k2_timedown = EEPROM.read(101);
      if (millis() - previousMillis > 1000)   // có thể dùng time của blynk
      {
        Blynk.virtualWrite(V5, analogRead(A0));
        Serial.print(analogRead(A0));
        previousMillis = millis();
        Serial.print(".");
        updateWebsocket();
      }
      if (millis() - previousOneMinuteCounter > 59550 && (EEPROM.read(100) != 0 || EEPROM.read(101) != 0) ) // 1 phút
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
        Blynk.virtualWrite(V4, "Mở khoá sau: "+mygio+" giờ "+myphut+" phút");
      }
      Blynk.run();
      timer.run();
    }
  }

 delay(10);
}
