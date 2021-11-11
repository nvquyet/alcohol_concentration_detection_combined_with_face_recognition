/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2021 mobizt
 *
*/

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

/* 1. Define the WiFi credentials */
#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

/* 2. Define the API Key */
#define API_KEY "API_KEY"

/* 3. Define the RTDB URL */
#define DATABASE_URL "URL" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

//Define Firebase Data objects
FirebaseData fbdo;
FirebaseData stream;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis1;

void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  //Or use legacy authenticate method
  //config.database_url = DATABASE_URL;
  //config.signer.tokens.legacy_token = "<database secret>";

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(stream, "/test/stream/data"))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());
}

void loop()
{

  //Flash string (PROGMEM and  (FPSTR), String C/C++ string, const char, char array, string literal are supported
  //in all Firebase and FirebaseJson functions, unless F() macro is not supported.

  if (!Firebase.ready())
    return;

  if (!Firebase.readStream(stream))
    Serial.printf("sream read error, %s\n\n", stream.errorReason().c_str());

  if (stream.streamTimeout())
  {
    Serial.println("stream timeout, resuming...\n");

    if (!stream.httpConnected())
      Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
  }

  if (stream.streamAvailable())
  {
    Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
                  stream.streamPath().c_str(),
                  stream.dataPath().c_str(),
                  stream.dataType().c_str(),
                  stream.eventType().c_str());
    printResult(stream); //see addons/RTDBHelper.h
    Serial.println();

    //This is the size of stream payload received (current and max value)
    //Max payload size is the payload size under the stream path since the stream connected
    //and read once and will not update until stream reconnection takes place.
    //This max value will be zero as no payload received in case of ESP8266 which
    //BearSSL reserved Rx buffer size is less than the actual stream payload.
    Serial.printf("Received stream payload size: %d (Max. %d)\n\n", stream.payloadLength(), stream.maxPayloadLength());
  }

  if (millis() - sendDataPrevMillis1 > 15000)
  {
    sendDataPrevMillis1 = millis();

    //Create demo data
    uint8_t data[256];
    for (int i = 0; i < 256; i++)
      data[i] = i;
    data[255] = rand();

    Serial.printf("Set BLOB... %s\n", Firebase.setBlob(fbdo, "/test/stream/data", data, sizeof(data)) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Free Heap, %d\n", (int)ESP.getFreeHeap());
    Serial.println();
  }
}