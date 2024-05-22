#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "PageIndex.h"

const char* ssid = "Seco";
const char* password = "secosanpq1";

ESP8266WebServer server(80);

// const int relayPin = 5; // GPIO 5 (D1)
#define outsideLed D1

void handleRoot() {
  String html = MAIN_page;
  server.send(200, "text/html", html);
}

void outsideLedOn() {
  digitalWrite(outsideLed, HIGH); 
  server.send(200, "text/plain", "Bec aprins");
}

void outsideLedOff() {
  digitalWrite(outsideLed, LOW); 

  server.send(200, "text/plain", "Bec stins");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(outsideLed, OUTPUT);
  // digitalWrite(outsideLed, LOW);// seteaza valoarea initiala

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", outsideLedOn);
  server.on("/off", outsideLedOff);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

