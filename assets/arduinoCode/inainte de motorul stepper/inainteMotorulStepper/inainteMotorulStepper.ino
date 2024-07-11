#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ShiftRegister74HC595.h>
#include "PageIndex.h"

// WiFi credentials
const char* ssid = "Seco";
const char* password = "secosanpq1";

// Create a web server object
ESP8266WebServer server(80);

// Pins for 74HC595
const int dataPin = D5;
const int clockPin = D6;
const int latchPin = D7;

// Create ShiftRegister74HC595 object with 2 shift registers
ShiftRegister74HC595<2> sr(dataPin, clockPin, latchPin);

// Define channels for LEDs
#define LED_QH 14
#define LED_QH_SECOND_REGISTER 15

// Define channels for the servo motors on the first shift register
#define FRONT_DOOR 0
#define BED_ROOM_DOOR 1
#define GUEST_DOOR 2

// Variables for servo control
int guestDoorPosition; // Initial position of the guest door servo
int frontDoorPosition; // Initial position of the front door servo
int bedRoomDoorPosition; // Initial position of the bedroom door servo
unsigned long lastServoUpdate = 0;
const int pwmFrequency = 50; // Servo PWM frequency in Hz (standard is 50Hz)
const int pwmPeriod = 1000 / pwmFrequency; // Period in milliseconds

// HTML page for the web server
void handleRoot() {
  String html = MAIN_page;
  server.send(200, "text/html", html);
}

// Functions to handle LED control
void ledOn() {
  sr.set(LED_QH_SECOND_REGISTER, true);
  server.send(200, "text/plain", "LED QH al doilea registru aprins");
}

void ledOff() {
  sr.set(LED_QH_SECOND_REGISTER, false);
  server.send(200, "text/plain", "LED QH al doilea registru stins");
}

void firstLedOn() {
  sr.set(LED_QH, true);
  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void firstLedOff() {
  sr.set(LED_QH, false);
  server.send(200, "text/plain", "LED QH primul registru stins");
}

// Functions to handle servo control
void setGuestDoorPosition(int position) {
  guestDoorPosition = position;
}

void openGuestDoor() {
  setGuestDoorPosition(175); // Adjust the angle as needed for opening the door
  server.send(200, "text/plain", "Guest door deschisă");
}

void closeGuestDoor() {
  setGuestDoorPosition(13); // Adjust the angle as needed for closing the door
  server.send(200, "text/plain", "Guest door închisă");
}

void setFrontDoorPosition(int position) {
  frontDoorPosition = position;
}

void openFrontDoor() {
  setFrontDoorPosition(140); // Adjust the angle as needed for opening the door
  server.send(200, "text/plain", "Front door deschisă");
}

void closeFrontDoor() {
  setFrontDoorPosition(15); // Adjust the angle as needed for closing the door
  server.send(200, "text/plain", "Front door închisă");
}

void setBedRoomDoorPosition(int position) {
  bedRoomDoorPosition = position;
}

void openBedRoomDoor() {
  setBedRoomDoorPosition(5); // Adjust the angle as needed for opening the door
  server.send(200, "text/plain", "Bedroom door deschisă");
}

void closeBedRoomDoor() {
  setBedRoomDoorPosition(150); // Adjust the angle as needed for closing the door
  server.send(200, "text/plain", "Bedroom door închisă");
}

void updateServoPWM() {
  unsigned long currentTime = millis();
  if (currentTime - lastServoUpdate >= pwmPeriod) {
    lastServoUpdate = currentTime;
    int guestDoorDutyCycle = map(guestDoorPosition, 0, 180, 544, 2400); // Map the position to servo PWM range (544 to 2400 microseconds)
    int frontDoorDutyCycle = map(frontDoorPosition, 0, 180, 544, 2400); // Map the position to servo PWM range (544 to 2400 microseconds)
    int bedRoomDutyCycle = map(bedRoomDoorPosition, 0, 180, 544, 2400); // Map the position to servo PWM range (544 to 2400 microseconds)
    
    // Update Guest Door Servo
    sr.set(GUEST_DOOR, true);
    delayMicroseconds(guestDoorDutyCycle); // High time for guest door servo
    sr.set(GUEST_DOOR, false);
    
    // Update Front Door Servo
    sr.set(FRONT_DOOR, true);
    delayMicroseconds(frontDoorDutyCycle); // High time for front door servo
    sr.set(FRONT_DOOR, false);

    // Update Bedroom Door Servo
    sr.set(BED_ROOM_DOOR, true);
    delayMicroseconds(bedRoomDutyCycle); // High time for bedroom door servo
    sr.set(BED_ROOM_DOOR, false);
    
    delayMicroseconds(pwmPeriod * 1000 - max(max(guestDoorDutyCycle, frontDoorDutyCycle), bedRoomDutyCycle)); // Low time
  }
}

// Setup function
void setup() {
  Serial.begin(115200);
  delay(500);

  sr.setAllLow();

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

  // Initialize servos to initial positions
  setGuestDoorPosition(13);
  setFrontDoorPosition(15);
  setBedRoomDoorPosition(150);

  // Define routes for the web server
  server.on("/", handleRoot);
  server.on("/on", ledOn);
  server.on("/off", ledOff);
  server.on("/firstLedOn", firstLedOn);
  server.on("/firstLedOff", firstLedOff);
  server.on("/openGuestDoor", openGuestDoor);
  server.on("/closeGuestDoor", closeGuestDoor);
  server.on("/openFrontDoor", openFrontDoor);
  server.on("/closeFrontDoor", closeFrontDoor);
  server.on("/openBedRoomDoor", openBedRoomDoor);
  server.on("/closeBedRoomDoor", closeBedRoomDoor);

  server.begin();
  Serial.println("HTTP server started");
}

// Main loop
void loop() {
  server.handleClient();
  updateServoPWM(); // Update the servo PWM signal
}



