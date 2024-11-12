#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Firebase.h>
#include <ShiftRegister74HC595.h>
#include "PageIndex.h"
#include <Firebase.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// WiFi credentials
const char* ssid = "Seco";
const char* password = "secosanpq1";


// Firebase configuration
// Firebase configuration
#define FIREBASE_HOST "licentalivedb-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "K9re6wdo8C5r860LD0TnPoSAo2I96n1NgMuFP3wJ"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;


// Create a web server object
ESP8266WebServer server(80);


#define LDR_PIN D0  
#define PIR_PIN D2  
#define DHT_PIN D1  

// Pins for 74HC595
const int dataPin = D5;
const int clockPin = D6;
const int latchPin = D7;

// Create ShiftRegister74HC595 object with 2 shift registers
ShiftRegister74HC595<3> sr(dataPin, clockPin, latchPin);

//Led
#define GARAGE_LEFT_LED 10
#define GARAGE_RIGHT_LED 11
#define GUEST_LED   12
#define DOOR_LEFT_LED 13
#define DOOR_RIGHT_LED 14
#define BEDROOM_LED 15
#define HALL_LED 16
#define LIVING_LED1 17
#define LIVING_LED2 18
//Fan
#define MOTOR_IN1 19  
#define MOTOR_IN2 20  
#define MOTOR_ENABLE 21 

// Define channels for the servo motors on the first shift register
#define FRONT_DOOR 0
#define BED_ROOM_DOOR 1
#define GUEST_DOOR 2
//Windows
#define GUEST_WINDOW 7
#define BED_ROOM_WINDOW 8
#define LIVING_WINDOW 9

// Define channels for stepper motor on the first shift register
#define IN1 3
#define IN2 4
#define IN3 5
#define IN4 6

//Temperatura
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

#define BUZZER_PIN D8
#define RAIN_SENSOR_PIN A0
#define FIRE_SENSOR_PIN D4
#define GAS_SENSOR_PIN D3

int ldrValue = 0;
int pirValue = 0;

// Stepper motor steps and delays
const int stepsPerRevolution = 2048; // 2048 steps per revolution for 28BYJ-48
const int stepsFor190Degrees = (stepsPerRevolution * 190) / 360;
const int stepperSpeedDelay = 5; // Adjust this value to change speed

volatile bool direction = true; // true = forward, false = backward
volatile bool isRunning = false;
volatile int stepsToMove = 0;
unsigned long lastStepperUpdate = 0;

// Variables for servo control
int guestDoorPosition; // Initial position of the guest door servo
int frontDoorPosition; // Initial position of the front door servo
int bedRoomDoorPosition; // Initial position of the bedroom door servo

int guestWindowPosition; // Initial position of the guest window servo
int bedRoomWindowPosition; // Initial position of the bedroom window servo
int livingWindowPosition; // Initial position of the living room window servo
unsigned long lastServoUpdate = 0;
const int pwmFrequency = 50; // Servo PWM frequency in Hz (standard is 50Hz)
const int pwmPeriod = 20; // Period in milliseconds (20ms for 50Hz)
const int pulseMin = 544; // Minimum pulse width in microseconds
const int pulseMax = 2400; // Maximum pulse width in microseconds


                bool windowsClosed = false; // Variabilă pentru starea geamurilor

// HTML page for the web server
void handleRoot() {
  String html = MAIN_page;
  server.send(200, "text/html", html);
}

// Handle temperature and humidity request just for html
void handleTemperatureHumidity() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Verificăm dacă citirile au fost valide
  if (isnan(t) || isnan(h)) {
    server.send(200, "text/plain", "{\"temperature\":\"Eroare\", \"humidity\":\"Eroare\"}");
    return;
  }

  String jsonResponse = "{\"temperature\":\"" + String(t) + "\", \"humidity\":\"" + String(h) + "\"}";
  server.send(200, "application/json", jsonResponse);
}

void checkFire() {
  if (digitalRead(FIRE_SENSOR_PIN) == LOW) {
    tone(BUZZER_PIN, 1000);
    Serial.println("Fire detected!");
    Firebase.setString(firebaseData, "/sensorData/fire", "Fire Detected!");

  } else {
    noTone(BUZZER_PIN);
    Firebase.setString(firebaseData, "/sensorData/fire", "No fire");

  }
}

void checkGas() {
  if (digitalRead(GAS_SENSOR_PIN) == LOW) {
    tone(BUZZER_PIN, 1000);
    Firebase.setString(firebaseData, "/sensorData/gas", "Gas Detected!");
    Serial.println("Gas detected!");
  } else {
    noTone(BUZZER_PIN);
    Firebase.setString(firebaseData, "/sensorData/gas", "No gas");

  }
}

void checkRain() {
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  Serial.print("Rain Sensor Value: ");
  Serial.println(rainValue);
  if (rainValue < 800) {
      Firebase.setString(firebaseData, "/sensorData/rain", "It's Raining!");
      closeWindows();
  } else {
      Firebase.setString(firebaseData, "/sensorData/rain", "No rain");

  }
}

void closeWindows() {
  Serial.println("Closing windows...");
    closeGuestWindow();
    closeLivingWindow();
    closeBedRoomWindow();
}

//Doar pentru HTML
void handleSensorsStatus() {
  String message = "Status: ";
  
  int fireState = digitalRead(FIRE_SENSOR_PIN);
  int gasState = digitalRead(GAS_SENSOR_PIN);
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  int pirValue = digitalRead(PIR_PIN);

  if (fireState == LOW) {
    message += "Fire detected! ";
    // Firebase.setString(firebaseData, "/sensorData/fire", "Fire Detected!");

  } else {
    message += "No fire. ";
    // Firebase.setString(firebaseData, "/sensorData/fire", "No fire");
  }

  if (gasState == LOW) {
    message += "Gas detected! ";
    // Firebase.setString(firebaseData, "/sensorData/gas", "Gas");

    // Firebase.RTDB.setString(&firebaseData, "/sensorData/gas", "Gas detected!");
    // if(Firebase.setString(firebaseData, "/sensorData/gas", "Gas Detected!")) {
    //   Serial.println("Gas updated successfully to firebase: ");
    // } else {
    //   Serial.print("Failed to update Gas to firebase: ");
    //   Serial.println(firebaseData.errorReason()); 
    // }
  } else {
    message += "No gas. ";
    // Firebase.setString(firebaseData, "/sensorData/gas", "No gas");

  }

  if (rainValue < 800) {
    message += "It's raining. ";
    // Firebase.setString(firebaseData, "/sensorData/rain", "It's Raining!");
    closeWindows();

  } else {
    // Firebase.setString(firebaseData, "/sensorData/rain", "No Rain");

    // if (Firebase.setString(firebaseData, "/sensorData/rain", "No Rain")) {
    //   Serial.println("Rain updated successfully to firebase: ");
    // } else {
    //   Serial.print("Failed to update rain to firebase: ");
    //   Serial.println(firebaseData.errorReason()); 
    // }
    message += "No rain. ";
  }

  if (pirValue == HIGH) {
    message += "Motion Detected!";
    // Firebase.setString(firebaseData, "/sensorData/motion", "Motion detected");

    // if (Firebase.setString(firebaseData, "/sensorData/motion", "Motion Detected!")) {
    //   Serial.println("Motion updated successfully to firebase: ");
    // } else {
    //   Serial.print("Failed to update motion to firebase: ");
    //   Serial.println(firebaseData.errorReason()); 
    // }
  } else {
    message += "No Motion. ";
    // Firebase.setString(firebaseData, "/sensorData/motion", "No Motion");
  }

  server.send(200, "text/plain", message);
}

// Functions to handle LED control
void turnOnGarageLed() {
  sr.set(GARAGE_LEFT_LED, true);
  sr.set(GARAGE_RIGHT_LED, true);
  server.send(200, "text/plain", "LED QH al doilea registru aprins");
}

void turnOffGarageLed() {
  sr.set(GARAGE_LEFT_LED, false);
  sr.set(GARAGE_RIGHT_LED, false);
  server.send(200, "text/plain", "LED QH primul registru stins");
}

void turnOnGuestLed() {
  sr.set(GUEST_LED, true);
  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void turnOffGuestLed() {
  sr.set(GUEST_LED, false);
  server.send(200, "text/plain", "LED QH primul registru stins");
}

void turnOnDoorLed() {
  sr.set(DOOR_LEFT_LED, true);
  sr.set(DOOR_RIGHT_LED, true);

  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void turnOffDoorLed() {
  sr.set(DOOR_LEFT_LED, false);
  sr.set(DOOR_RIGHT_LED, false);

  server.send(200, "text/plain", "LED QH primul registru stins");
}


void turnOnBedroomLed() {
  sr.set(BEDROOM_LED, true);
  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void turnOffBedroomLed() {
  sr.set(BEDROOM_LED, false);
  server.send(200, "text/plain", "LED QH primul registru aprins");
}


void turnOnHallLed() {
  sr.set(HALL_LED, true);
  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void turnOffHallLed() {
  sr.set(HALL_LED, false);
  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void turnOnLivingLed1() {
  sr.set(LIVING_LED1, true);
  sr.set(LIVING_LED2, true);

  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void turnOffLivingLed1() {
  sr.set(LIVING_LED1, false);
  sr.set(LIVING_LED2, false); 
  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void turnOnLivingLed2() {
  sr.set(LIVING_LED2, true);
  server.send(200, "text/plain", "LED QH primul registru aprins");
}

void turnOffLivingLed2() {
  sr.set(LIVING_LED2, false); 
  server.send(200, "text/plain", "LED QH primul registru aprins");
}



// Functions to handle servo control
void setGuestDoorPosition(int position) {
  guestDoorPosition = position;
}

void openGuestDoor() {
  setGuestDoorPosition(170); // Adjust the angle as needed for opening the door
  server.send(200, "text/plain", "Guest door deschisă");
}

void closeGuestDoor() {
  setGuestDoorPosition(10); // Adjust the angle as needed for closing the door
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
  setBedRoomDoorPosition(145); // Adjust the angle as needed for closing the door
  server.send(200, "text/plain", "Bedroom door închisă");
}

// Guest Window
void setGuestWindowPosition(int position) {
  guestWindowPosition = position;
}

void openGuestWindow() {
  setGuestWindowPosition(25);
  server.send(200, "text/plain", "Guest window deschisă");
}

void closeGuestWindow() {
  setGuestWindowPosition(110);
  server.send(200, "text/plain", "Guest window închisă");
}

// Bed Room Window
void setBedRoomWindowPosition(int position) {
  bedRoomWindowPosition = position;
}

void openBedRoomWindow() {
  setBedRoomWindowPosition(130);
  server.send(200, "text/plain", "Bedroom window deschisă");
}

void closeBedRoomWindow() {
  setBedRoomWindowPosition(20);
  server.send(200, "text/plain", "Bedroom window închisă");
}

// Living Window
void setLivingWindowPosition(int position) {
  livingWindowPosition = position;
}

void openLivingWindow() {
  setLivingWindowPosition(115);
  server.send(200, "text/plain", "Living window deschisă");
}

void closeLivingWindow() {
  setLivingWindowPosition(10);
  server.send(200, "text/plain", "Living window închisă");
}

void updateServoPWM() {
  unsigned long currentTime = millis();
  if (currentTime - lastServoUpdate >= pwmPeriod) {
    lastServoUpdate = currentTime;

    int guestDoorPulse = map(guestDoorPosition, 0, 180, pulseMin, pulseMax);
    int frontDoorPulse = map(frontDoorPosition, 0, 180, pulseMin, pulseMax);
    int bedRoomPulse = map(bedRoomDoorPosition, 0, 180, pulseMin, pulseMax);
    int guestWindowPulse = map(guestWindowPosition, 0, 180, pulseMin, pulseMax);
    int livingWindowPulse = map(livingWindowPosition, 0, 180, pulseMin, pulseMax);
    int bedRoomWindowPulse = map(bedRoomWindowPosition, 0, 180, pulseMin, pulseMax);

    // Send PWM signal to Guest Door Servo
    sr.set(GUEST_DOOR, true);
    delayMicroseconds(guestDoorPulse);
    sr.set(GUEST_DOOR, false);

    // Send PWM signal to Front Door Servo
    sr.set(FRONT_DOOR, true);
    delayMicroseconds(frontDoorPulse);
    sr.set(FRONT_DOOR, false);

    // Send PWM signal to Bedroom Door Servo
    sr.set(BED_ROOM_DOOR, true);
    delayMicroseconds(bedRoomPulse);
    sr.set(BED_ROOM_DOOR, false);
    
    // Send PWM signal to Guest Window Servo
    sr.set(GUEST_WINDOW, true);
    delayMicroseconds(guestWindowPulse);
    sr.set(GUEST_WINDOW, false);

    // Send PWM signal to Living Window Servo
    sr.set(LIVING_WINDOW, true);
    delayMicroseconds(livingWindowPulse);
    sr.set(LIVING_WINDOW, false);

    // Send PWM signal to Bedroom Window Servo
    sr.set(BED_ROOM_WINDOW, true);
    delayMicroseconds(bedRoomWindowPulse);
    sr.set(BED_ROOM_WINDOW, false);
  }
}

// Functions to handle stepper motor control
void stepMotor(int step) {
  switch (step) {
    case 0:
      sr.set(IN1, true);
      sr.set(IN2, false);
      sr.set(IN3, false);
      sr.set(IN4, false);
      break;
    case 1:
      sr.set(IN1, false);
      sr.set(IN2, true);
      sr.set(IN3, false);
      sr.set(IN4, false);
      break;
    case 2:
      sr.set(IN1, false);
      sr.set(IN2, false);
      sr.set(IN3, true);
      sr.set(IN4, false);
      break;
    case 3:
      sr.set(IN1, false);
      sr.set(IN2, false);
      sr.set(IN3, false);
      sr.set(IN4, true);
      break;
  }
}

void deactivateMotor() {
  sr.set(IN1, false);
  sr.set(IN2, false);
  sr.set(IN3, false);
  sr.set(IN4, false);
}

void stepperTask() {
  if (isRunning && stepsToMove > 0) {
    unsigned long currentTime = millis();
    if (currentTime - lastStepperUpdate >= stepperSpeedDelay) {
      lastStepperUpdate = currentTime;
      static int currentStep = 0;
      stepMotor(currentStep);
      if (direction) {
        currentStep++;
        if (currentStep > 3) {
          currentStep = 0;
        }
      } else {
        currentStep--;
        if (currentStep < 0) {
          currentStep = 3;
        }
      }
      stepsToMove--;
      if (stepsToMove == 0) {
        isRunning = false;
        deactivateMotor(); // Deactivate motor when done
      }
    }
  }
}

void moveForward() {
  stepsToMove = stepsFor190Degrees;
  direction = true;
  isRunning = true;
  server.send(200, "text/plain", "Moving 190 degrees forward");
}

void moveBackward() {
  stepsToMove = stepsFor190Degrees;
  direction = false;
  isRunning = true;
  server.send(200, "text/plain", "Moving 190 degrees backward");
}

unsigned long lastMotionTime = 0;  // Variabilă pentru a stoca timpul ultimei mișcări detectate
const unsigned long lightOnDuration = 5000;  // Durata în milisecunde (5 secunde)

void checkLightAndMotion() {
  ldrValue = digitalRead(LDR_PIN);
  pirValue = digitalRead(PIR_PIN);
  Serial.print("PIR: ");
  Serial.print(pirValue);
 
  // Verificăm dacă este întuneric și se detectează mișcare
  if (ldrValue == HIGH && pirValue == HIGH) {
    turnOnHallLed();
    lastMotionTime = millis();  
  }

  // Verificăm dacă au trecut 10 secunde de la ultima mișcare detectată
  if (millis() - lastMotionTime > lightOnDuration) {
    turnOffHallLed();
  }

  if (pirValue == HIGH){
    Firebase.setString(firebaseData, "/sensorData/motion", "Motion Detected!");
  } else if (pirValue == LOW)
  {
    Firebase.setString(firebaseData, "/sensorData/motion", "No Motion");
  }
  
}

void doorLightEvening() {
  ldrValue = digitalRead(LDR_PIN);
  // Verificăm dacă este întuneric 
  if (ldrValue == HIGH) {
    turnOnDoorLed();
    Firebase.setString(firebaseData, "/sensorData/isDay", "is Night " + String(ldrValue));

  } else {
    turnOffDoorLed();
    Firebase.setString(firebaseData, "/sensorData/isDay", "is Day " + String(ldrValue));
  }
}

void displayTemperatureHumidity() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Verificăm dacă citirea a fost validă
  if (isnan(h) || isnan(t)) {
    Serial.println("Eroare la citirea DHT11!");
    return;
  }

  Serial.print("Umiditate: ");
  Serial.print(h);
  Serial.print("%  Temperatura: ");
  Serial.print(t);
  Serial.println("°C");
  
  // Trimite datele senzorilor la Firebase
  Firebase.setString(firebaseData, "/sensorData/temperature", t);
  Firebase.setString(firebaseData, "/sensorData/humidity", h);


  // if (Firebase.setFloat(firebaseData, "/sensorData/temperature", t)) {
  //   Serial.println("Temperature updated successfully to firebase: " + String(t));
  // } else {
  //   Serial.print("Failed to update temperature to firebase: ");
  //   Serial.println(firebaseData.errorReason()); 
  // }

  //   if (Firebase.setFloat(firebaseData, "/sensorData/humidity", h)) {
  //   Serial.println("Humidity updated successfully to firebase: " + String(h));
  // } else {
  //   Serial.print("Failed to update humidity to firebase: ");
  //   Serial.println(firebaseData.errorReason()); 
  // }
}

//Fan
void startFan() {
  sr.set(MOTOR_ENABLE, HIGH); 
  sr.set(MOTOR_IN1, HIGH);  // Setează IN1 la HIGH (direcția înainte)
  sr.set(MOTOR_IN2, LOW);   // Setează IN2 la LOW
}


void stopFan() {
  // Setează pinii de direcție și pinul enable la LOW pentru a opri motorul
  sr.set(MOTOR_IN1, LOW);
  sr.set(MOTOR_IN2, LOW);
  sr.set(MOTOR_ENABLE, LOW);  // Oprește complet motorul
}
FirebaseData fbdo;
FirebaseData stream;

String parentPath = "/";  
String childPath[16] = {
  "/led/garageLed",
  "/led/guestLed",
  "/led/doorLed",
  "/led/bedRoomLed",
  "/led/hallLed",
  "/led/livingLed",
  "/door/guestDoor",
  "/door/frontDoor",
  "/door/bedRoomDoor",
  "/door/garage",
  "/window/guestWindow",
  "/window/livingWindow",
  "/window/bedRoomWindow",
  "/motor/fan",
  "/status/message"
};

// Variabilă pentru a indica dacă datele s-au schimbat
volatile bool dataChanged = false;

void streamCallback(MultiPathStreamData stream) {
  size_t numChild = sizeof(childPath) / sizeof(childPath[0]);

  for (size_t i = 0; i < numChild; i++) {
    if (stream.get(childPath[i])) {
      String path = stream.dataPath;
      String value = stream.value.c_str();

      Serial.printf("path: %s, event: %s, type: %s, value: %s%s",
                    path.c_str(), stream.eventType.c_str(),
                    stream.type.c_str(), value.c_str(),
                    i < numChild - 1 ? "\n" : "");

      // Control LED-uri
      if (path == "/led/garageLed") {
        value == "on" ? turnOnGarageLed() : turnOffGarageLed();
      } else if (path == "/led/guestLed") {
        value == "on" ? turnOnGuestLed() : turnOffGuestLed();
      } else if (path == "/led/doorLed") {
        value == "on" ? turnOnDoorLed() : turnOffDoorLed();
      } else if (path == "/led/bedRoomLed") {
        value == "on" ? turnOnBedroomLed() : turnOffBedroomLed();
      } else if (path == "/led/hallLed") {
        value == "on" ? turnOnHallLed() : turnOffHallLed();
      } else if (path == "/led/livingLed") {
        if (value == "on") {
          turnOnLivingLed1();
          turnOnLivingLed2();
        } else {
          turnOffLivingLed1();
          turnOffLivingLed2();
        }
      }

      // Control uși
      else if (path == "/door/guestDoor") {
        value == "open" ? openGuestDoor() : closeGuestDoor();
      } else if (path == "/door/frontDoor") {
        value == "open" ? openFrontDoor() : closeFrontDoor();
      } else if (path == "/door/bedRoomDoor") {
        value == "open" ? openBedRoomDoor() : closeBedRoomDoor();
      } else if (path == "/door/garage") {
        value == "forward" ? moveForward() : moveBackward();
      }

      // Control ferestre
      else if (path == "/window/guestWindow") {
        value == "open" ? openGuestWindow() : closeGuestWindow();
      } else if (path == "/window/livingWindow") {
        value == "open" ? openLivingWindow() : closeLivingWindow();
      } else if (path == "/window/bedRoomWindow") {
        value == "open" ? openBedRoomWindow() : closeBedRoomWindow();
      }

      // Control motor
      else if (path == "/motor/fan") {
        value == "start" ? startFan() : stopFan();
      }

      // Status mesaj
      else if (path == "/status/message") {
        Serial.println("Status message: " + value);
      }
    }
  }
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", stream.payloadLength(), stream.maxPayloadLength());
  dataChanged = true;
}


void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timed out, resuming...");
  }

  if (!stream.httpConnected()) {
    Serial.printf("Error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
  }
}
// Setup function
void setup() {
  Serial.begin(115200);
  dht.begin();

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

  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  
  // Setarea buffer-ului pentru ESP8266
  fbdo.setBSSLBufferSize(4096, 1024);
  stream.setBSSLBufferSize(2048, 512);

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Setarea MultiPath Stream
if (!Firebase.beginMultiPathStream(stream, parentPath)) {
    Serial.printf("Stream begin error, %s\n\n", stream.errorReason().c_str());
  }

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);

  Serial.println("Connected to Firebase");


  // Initialize servos to initial positions
  setGuestDoorPosition(10);
  setFrontDoorPosition(15);
  setBedRoomDoorPosition(145);
  //Window
  setGuestWindowPosition(110);
  setLivingWindowPosition(10);
  setBedRoomWindowPosition(20);


  pinMode(LDR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);

  // Define routes for the web server
  server.on("/", handleRoot);
  server.on("/temperature_humidity", handleTemperatureHumidity);
  server.on("/sensors_status", handleSensorsStatus);

  // LED control routes
  server.on("/garageLedOn", turnOnGarageLed);
  server.on("/garageLedOff", turnOffGarageLed);
  server.on("/guestLedOn", turnOnGuestLed);
  server.on("/guestLedOff", turnOffGuestLed);
  server.on("/doorLeftLedOn", turnOnDoorLed);
  server.on("/doorLeftLedOff", turnOffDoorLed);
  server.on("/bedRoomLedOn", turnOnBedroomLed);
  server.on("/bedRoomLedOff", turnOffBedroomLed);
  server.on("/hallLedOn", turnOnHallLed);
  server.on("/hallLedOff", turnOffHallLed);
  server.on("/livingLedOn", []() {turnOnLivingLed1(); turnOnLivingLed2();});
  server.on("/livingLedOff", []() {turnOffLivingLed1(); turnOffLivingLed2();});


  // Door control routes
  server.on("/openGuestDoor", openGuestDoor);
  server.on("/closeGuestDoor", closeGuestDoor);
  server.on("/openFrontDoor", openFrontDoor);
  server.on("/closeFrontDoor", closeFrontDoor);
  server.on("/openBedRoomDoor", openBedRoomDoor);
  server.on("/closeBedRoomDoor", closeBedRoomDoor);

  // Window control routes
  server.on("/openGuestWindow", openGuestWindow);
  server.on("/closeGuestWindow", closeGuestWindow);
  server.on("/openLivingWindow", openLivingWindow);
  server.on("/closeLivingWindow", closeLivingWindow);
  server.on("/openBedRoomWindow", openBedRoomWindow);
  server.on("/closeBedRoomWindow", closeBedRoomWindow);

  // Stepper motor routes
  server.on("/moveForward", moveForward);
  server.on("/moveBackward", moveBackward);
  

  // Motor control routes (for L293D motor)
  server.on("/startFan", startFan);
  server.on("/stopFan", stopFan);


  server.begin();
  Serial.println("HTTP server started");
}

// Main loop
// Timere pentru senzori 
unsigned long lastSensorReadTime = 0;
const unsigned long sensorReadInterval = 5000; // 5 secunde

void loop() {
  // Gestionare server și alte funcții
  server.handleClient();
  updateServoPWM(); 
  stepperTask();

  unsigned long currentTime = millis();

  // Citirea senzorilor la intervalul definit
  if (currentTime - lastSensorReadTime >= sensorReadInterval) {
    lastSensorReadTime = currentTime;
    checkLightAndMotion();
    doorLightEvening();
    displayTemperatureHumidity();
    checkFire();
    checkGas();
    checkRain();
  }

  //  dacă datele s-au schimbat
  if (Firebase.ready() && dataChanged) {
    dataChanged = false;
    //Adauga alte actiuni cand datele se schimba
  }

#if !defined(ESP8266) && !defined(ESP32)
  Firebase.runStream();
#endif

}