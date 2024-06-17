#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Servo.h>

#include "PageIndex.h"

const char* ssid = "Seco";
const char* password = "secosanpq1";

ESP8266WebServer server(80);

#define outsideLed D1
Servo frontDoor;
Servo bedRoomDoor;
Servo guestDoor;

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
  int pos = 155;
  if (pos >= 0 && pos <= 180) {
    // Inversează valoarea unghiului
    //pos = 180 - pos;
    frontDoor.write(pos);
    delay(15);
    // Serial.print("Servo Angle: ");
    // Serial.println(pos);
 
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
  }
}

void closeFrontDoor() {
  int pos = 30;
  if (pos >= 0 && pos <= 180) {
    // Inversează valoarea unghiului
   // pos = 180 - pos;
    frontDoor.write(pos);
    delay(15);
    // Serial.print("Servo Angle: ");
    // Serial.println(pos);
 
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
  }
}


void openBedRoomDoor() {
  int pos = 0;
  if (pos >= 0 && pos <= 180) {
    // Inversează valoarea unghiului
    // pos = 180 - pos;
    bedRoomDoor.write(pos);
    delay(15);
    // Serial.print("Servo Angle: ");
    // Serial.println(pos);
 
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
  }
}

void closeBedRoomDoor() {
  int pos = 126;
  if (pos >= 0 && pos <= 180) {
    // Inversează valoarea unghiului
    // pos = 180 - pos;
    bedRoomDoor.write(pos);
    delay(15);
    // Serial.print("Servo Angle: ");
    // Serial.println(pos);
 
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
  }
}



void openGuestDoor() {
  int pos = 160;
  if (pos >= 0 && pos <= 180) {
    // Inversează valoarea unghiului
    // pos = 180 - pos;
    guestDoor.write(pos);
    delay(15);
    // Serial.print("Servo Angle: ");
    // Serial.println(pos);
 
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
  }
}

void closeGuestDoor() {
  int pos = 6;
  if (pos >= 0 && pos <= 180) {
    // Inversează valoarea unghiului
    // pos = 180 - pos;
    guestDoor.write(pos);
    delay(15);
    // Serial.print("Servo Angle: ");
    // Serial.println(pos);
 
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
  }
}

void setServo() {
  if (server.hasArg("servo") && server.hasArg("angle")) {
    String servo = server.arg("servo");
    int pos = server.arg("angle").toInt();
    if (pos >= 0 && pos <= 180) {
      if (servo == "frontDoor") {
        frontDoor.write(pos);
      } else if (servo == "guestDoor") {
        guestDoor.write(pos);
      } else if (servo == "bedRoom") {
        bedRoomDoor.write(pos);
      } else {
        server.send(400, "text/plain", "Unknown servo identifier.");
        return;
      }
      delay(15);
      server.send(200, "text/plain", "Servo " + servo + " set to " + pos);
    } else {
      server.send(400, "text/plain", "Invalid position value. Position must be between 0 and 180.");
    }
  } else {
    server.send(400, "text/plain", "Both 'servo' and 'angle' parameters are required.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(outsideLed, OUTPUT);
  // digitalWrite(outsideLed, LOW);// seteaza valoarea initiala
  frontDoor.attach(D3, 500, 2400);   ////la esp8266 e problema si daca scrii normal adica doar D1, motorul merge maxim 90 de grade
  guestDoor.attach(D1, 500, 2400);   ////la esp8266 e problema si daca scrii normal adica doar D1, motorul merge maxim 90 de grade
  bedRoomDoor.attach(D2, 500, 2400);   ////la esp8266 e problema si daca scrii normal adica doar D1, motorul merge maxim 90 de grade
  guestDoor.write(6);
  bedRoomDoor.write(126);
  frontDoor.write(30);


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
  server.on("/setServo", setServo);
  server.on("/openBedRoomDoor", openBedRoomDoor);
  server.on("/closeBedRoomDoor", closeBedRoomDoor);
  server.on("/openGuestDoor", openGuestDoor);
  server.on("/closeGuestDoor", closeGuestDoor);


  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}






