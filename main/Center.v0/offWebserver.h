#ifndef offWebserver_h
#define offWebserver_h
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include "offHtml.h"
extern ESP8266WebServer server;

void handleWebsite(void) {
  server.send(200, "text/html", PAGE_NetworkConfiguration); //gửi dưới dạng html
}

/* Ham cai dat de luu ten va mat khau wifi vao ROM */
void cai_dat(void) {
  String tenwf = server.arg("tenWiFi");
  String mk = server.arg("matkhau");
  Serial.print("ten wifi:");
  Serial.println(tenwf);
  Serial.print("mat khau:");
  Serial.println(mk);
  /* Kiem tra ten va mk doc duoc co hay khong */
  if (tenwf.length() > 0 && mk.length() > 0) {
    Serial.println("clear EEPROM");
    // write a 0 to 96 bytes of the EEPROM
    for (int i = 0; i < 96; ++i) {
      EEPROM.write(i, 0);
    }
    delay(400);
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
    delay(400);
    Serial.println("Commit");
    EEPROM.commit();
    delay(200);
    Serial.println("Reset");
  }
  /* Sau khi luu xong reset chuong trinh */
  ESP.reset();
}

/* Ham test ket noi wifi luc dau */
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

/* check connect to wifi */
bool check_connect(void)
{
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  else {
    return false;
  }
}
#endif
