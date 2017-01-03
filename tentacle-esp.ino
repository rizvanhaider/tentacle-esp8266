
#include <SPI.h>
#include <ESP8266WiFi.h>

//Warning: If you are using the official Arduino wifi shield, not defining this variable may produce corrupted data.
//Defining it, however, slows things down quite a bit.
#define MIN_DELAY 2000

#include "tentacle-build.h"

//octoblu hq
const char* ssid  = "SSID";
const char* password  = "PASSWORD";
#define server "192.168.100.19" // YOUR MACHING IP WHERE MESHBLU SERVER IS RUNNING OR tentacle.octoblu.com for live
/*#include "wifi-credentials.h"*/
#define port 3000 // 80 for live

static const char uuid[]  = "DEVICE UUID";
static const char token[] = "DEVICE TOKEN";

int status = WL_IDLE_STATUS;
WiFiClient conn;

TentacleArduino tentacle;
Pseudopod pseudopod(conn, conn, tentacle);

void delayIfNeeded() {
#ifdef MIN_DELAY
delay(MIN_DELAY);
#endif
}

void delayTheAppropriateTime() {
  #ifdef MIN_DELAY
  if(pseudopod.getBroadcastInterval() < MIN_DELAY) {
    delay(MIN_DELAY);
  } else {
    delay(pseudopod.getBroadcastInterval());
  }
  #else
  delay(pseudopod.getBroadcastInterval());
  #endif
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("The Day of the Tentacle has begun!"));

  setupWifi();
  connectToServer();
  delayIfNeeded();
}

void loop() {
//  if (!conn.available()) {
//    conn.stop();
//    connectToServer();
//  }

  readData();
  if(pseudopod.shouldBroadcastPins() ) {
    delayTheAppropriateTime();
    Serial.println(F("Sending pins"));
    pseudopod.sendConfiguredPins();
  }
}

void readData() {
  delayIfNeeded();

  while (conn.available()) {
    Serial.println(F("Received message"));
    Serial.flush();

    if(pseudopod.readMessage() == TentacleMessageTopic_action) {
      delayIfNeeded();
      pseudopod.sendPins();
    }
  }

  delayIfNeeded();
}

void connectToServer() {
  int connectionAttempts = 0;
  Serial.println(F("Connecting to the server."));
  Serial.flush();

  while(!conn.connect(server, port)) {
    if(connectionAttempts > 5) {
      Serial.println(F("Still can't connect. I must have gone crazy. Rebooting"));
      Serial.flush();
      ESP.reset();
    }
    Serial.println(F("Can't connect to the server."));
    Serial.flush();
    conn.stop();
    delayIfNeeded();
    connectionAttempts++;
  }

  size_t authSize = pseudopod.authenticate(uuid, token);
  Serial.print(authSize);
  Serial.println(F(" bytes written for authentication"));

}

void setupWifi() {
    WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
//  int status = WL_IDLE_STATUS;
//
//  while (status != WL_CONNECTED) {
//    Serial.print(F("Attempting to connect to SSID: "));
//    Serial.println(ssid);
//    Serial.flush();
//    status = WiFi.begin(ssid, password);
//  }

  // print the SSID of the network you're attached to:
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  Serial.flush();
}


