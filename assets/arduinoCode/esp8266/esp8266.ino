#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Servo.h>

#include "PageIndex.h"

const char* ssid = "Seco";
const char* password = "secosanpq1";

ESP8266WebServer server(80);

#define outsideLed D1
Servo frontDoor;

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

void openFrontDoor() {
  int pos = 90;
  if (pos >= 0 && pos <= 180) {
    // Inversează valoarea unghiului
    pos = 180 - pos;
    frontDoor.write(pos);
    delay(15);
    Serial.print("Servo Angle: ");
    Serial.println(pos);
 
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
  }
}

void closeFrontDoor() {
  int pos = 0;
  if (pos >= 0 && pos <= 180) {
    // Inversează valoarea unghiului
    pos = 180 - pos;
    frontDoor.write(pos);
    delay(15);
    Serial.print("Servo Angle: ");
    Serial.println(pos);
 
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(outsideLed, OUTPUT);
  // digitalWrite(outsideLed, LOW);// seteaza valoarea initiala
  frontDoor.attach(D2, 500, 2400);   ////la esp8266 e problema si daca scrii normal adica doar D1, motorul merge maxim 90 de grade

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
  server.on("/openFrontDoor", openFrontDoor);
  server.on("/closeFrontDoor", closeFrontDoor);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

