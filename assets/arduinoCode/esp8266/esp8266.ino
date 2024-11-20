#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Firebase.h>
#include <ShiftRegister74HC595.h>
#include "PageIndex.h"
#include <Firebase.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "bipolarstepper.h"

// WiFi credentials
const char* ssid = "Seco";
const char* password = "secosanpq1";


// Firebase configuration
// Firebase configuration
#define FIREBASE_HOST "licentalivedb-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "K9re6wdo8C5r860LD0TnPoSAo2I96n1NgMuFP3wJ"
volatile bool isStepperMotorRunning = false; // Indică dacă motorul este activ

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

BiPolStepper stepper(2048); // Motor pas cu pas cu 2048 pași


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
// const int pwmPeriod = 20; // Period in milliseconds (20ms for 50Hz)
const unsigned long pwmPeriod = 20000; // 20ms în microsecunde

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
  // Serial.print("Rain Sensor Value: ");
  // Serial.println(rainValue);
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


int lastGuestDoorPosition = -1; // Poziția anterioară a ușii de oaspeți
int lastFrontDoorPosition = -1;
int lastBedRoomDoorPosition = -1;
int lastGuestWindowPosition = -1;
int lastLivingWindowPosition = -1;
int lastBedRoomWindowPosition = -1;

void setGuestDoorPosition(int position) {
    if (position != lastGuestDoorPosition) { // Actualizăm doar dacă poziția s-a schimbat
        guestDoorPosition = position;
        Serial.println("Încep actualizarea PWM la guestDoor...");

        updateServoPWM(GUEST_DOOR, position); // Trimite semnal PWM doar pentru acest servo
                Serial.println("Gata actualizarea PWM la guestDoor...");

        lastGuestDoorPosition = position;    // Actualizăm poziția curentă
    }
}

void openGuestDoor() {
  setGuestDoorPosition(170); // Adjust the angle as needed for opening the door

  server.send(200, "text/plain", "Guest door deschisă");
}

void closeGuestDoor() {
  setGuestDoorPosition(10); // Adjust the angle as needed for closing the door
    //  updateServoPWM();

  server.send(200, "text/plain", "Guest door închisă");
}



void setFrontDoorPosition(int position) {
    if (position != lastFrontDoorPosition) { // Verificăm dacă poziția este diferită
        frontDoorPosition = position;
                Serial.println("Încep actualizarea PWM la frontDoor...");

        updateServoPWM(FRONT_DOOR, position); // Actualizăm doar acest servo
                        Serial.println("gata actualizarea PWM la frontDoor...");

        lastFrontDoorPosition = position;    // Actualizăm poziția curentă
    }
}

void openFrontDoor() {
  setFrontDoorPosition(140); // Adjust the angle as needed for opening the door

  server.send(200, "text/plain", "Front door deschisă");
}

void closeFrontDoor() {
  setFrontDoorPosition(15); // Adjust the angle as needed for closing the door
    //  updateServoPWM();

  server.send(200, "text/plain", "Front door închisă");
}


void setBedRoomDoorPosition(int position) {
    if (position != lastBedRoomDoorPosition) { // Verificăm dacă poziția este diferită
        bedRoomDoorPosition = position;
                        Serial.println("Încep actualizarea PWM la bedRoomDoor...");

        updateServoPWM(BED_ROOM_DOOR, position); // Actualizăm doar acest servo
                                Serial.println("Gata actualizarea PWM la bedRoomDoor...");

        lastBedRoomDoorPosition = position;      // Actualizăm poziția curentă
    }
}


void openBedRoomDoor() {
  setBedRoomDoorPosition(5); // Adjust the angle as needed for opening the door

  server.send(200, "text/plain", "Bedroom door deschisă");
}

void closeBedRoomDoor() {
  setBedRoomDoorPosition(145); // Adjust the angle as needed for closing the door
    //  updateServoPWM();

  server.send(200, "text/plain", "Bedroom door închisă");
}


// Guest Window
void setGuestWindowPosition(int position) {
    if (position != lastGuestWindowPosition) { // Verificăm dacă poziția este diferită
        guestWindowPosition = position;
                                Serial.println("Încep actualizarea PWM la guestWindow...");

        updateServoPWM(GUEST_WINDOW, position); // Actualizăm doar acest servo
                                        Serial.println("Gata actualizarea PWM la guestWindow...");

        lastGuestWindowPosition = position;     // Actualizăm poziția curentă
    }
}


void openGuestWindow() {
  setGuestWindowPosition(25);

  server.send(200, "text/plain", "Guest window deschisă");
}

void closeGuestWindow() {
  setGuestWindowPosition(110);
    //  updateServoPWM();

  server.send(200, "text/plain", "Guest window închisă");
}
// Bed Room Window
void setBedRoomWindowPosition(int position) {
    if (position != lastBedRoomWindowPosition) { // Verificăm dacă poziția este diferită
        bedRoomWindowPosition = position;
                                        Serial.println("Încep actualizarea PWM la bedRoomWindow...");

        updateServoPWM(BED_ROOM_WINDOW, position); // Actualizăm doar acest servo
                                                Serial.println("Gata actualizarea PWM la bedRoomWindow...");

        lastBedRoomWindowPosition = position;      // Actualizăm poziția curentă
    }
}


void openBedRoomWindow() {
  setBedRoomWindowPosition(130);

  server.send(200, "text/plain", "Bedroom window deschisă");
}

void closeBedRoomWindow() {
  setBedRoomWindowPosition(20);
    //  updateServoPWM();

  server.send(200, "text/plain", "Bedroom window închisă");
}


// Living Window
void setLivingWindowPosition(int position) {
    if (position != lastLivingWindowPosition) { // Verificăm dacă poziția este diferită
        livingWindowPosition = position;
                                                Serial.println("Încep actualizarea PWM la livingWindow...");

        updateServoPWM(LIVING_WINDOW, position); // Actualizăm doar acest servo
                                                Serial.println("gata actualizarea PWM la livingWindow...");

        lastLivingWindowPosition = position;     // Actualizăm poziția curentă
    }
}


void openLivingWindow() {
  setLivingWindowPosition(115);

  server.send(200, "text/plain", "Living window deschisă");
}

void closeLivingWindow() {
  setLivingWindowPosition(10);
    //  updateServoPWM();

  server.send(200, "text/plain", "Living window închisă");
}

// void updateServoPWM(int channel, int position) {
//     int pulseWidth = map(position, 0, 180, pulseMin, pulseMax);

//     // Trimite impulsurile PWM doar pentru canalul specific
//     sr.set(channel, true);
//     delayMicroseconds(pulseWidth);
//     sr.set(channel, false);
// }

void updateServoPWM(int channel, int position) {
    int pulseWidth = map(position, 0, 180, pulseMin, pulseMax);
    unsigned long startTime = micros();

    sr.set(channel, true);

    while (micros() - startTime < pulseWidth) {
        yield();
        if (micros() - startTime > 5000) { // Timeout de siguranță
            break;
        }
    }

    sr.set(channel, false);
}


// void updateServoPWM() {
//     int guestDoorPulse = map(guestDoorPosition, 0, 180, pulseMin, pulseMax);
//     int frontDoorPulse = map(frontDoorPosition, 0, 180, pulseMin, pulseMax);
//     int bedRoomPulse = map(bedRoomDoorPosition, 0, 180, pulseMin, pulseMax);
//     int guestWindowPulse = map(guestWindowPosition, 0, 180, pulseMin, pulseMax);
//     int livingWindowPulse = map(livingWindowPosition, 0, 180, pulseMin, pulseMax);
//     int bedRoomWindowPulse = map(bedRoomWindowPosition, 0, 180, pulseMin, pulseMax);

//     // Trimite impulsurile PWM pentru fiecare servo
//     sr.set(GUEST_DOOR, true);
//     delayMicroseconds(guestDoorPulse);
//     sr.set(GUEST_DOOR, false);

//     sr.set(FRONT_DOOR, true);
//     delayMicroseconds(frontDoorPulse);
//     sr.set(FRONT_DOOR, false);

//     sr.set(BED_ROOM_DOOR, true);
//     delayMicroseconds(bedRoomPulse);
//     sr.set(BED_ROOM_DOOR, false);

//     sr.set(GUEST_WINDOW, true);
//     delayMicroseconds(guestWindowPulse);
//     sr.set(GUEST_WINDOW, false);

//     sr.set(LIVING_WINDOW, true);
//     delayMicroseconds(livingWindowPulse);
//     sr.set(LIVING_WINDOW, false);

//     sr.set(BED_ROOM_WINDOW, true);
//     delayMicroseconds(bedRoomWindowPulse);
//     sr.set(BED_ROOM_WINDOW, false);
// }






// void deactivateMotor() {
//   sr.set(IN1, false);
//   sr.set(IN2, false);
//   sr.set(IN3, false);
//   sr.set(IN4, false);
// }
void setNewStepperState() {
  // Actualizăm starea pinii IN1, IN2, IN3 și IN4
  sr.set(IN1, stepper.state[0]);
  sr.set(IN2, stepper.state[1]);
  sr.set(IN3, stepper.state[2]);
  sr.set(IN4, stepper.state[3]);

  // Trimite datele către shift register
  sr.updateRegisters();
}


void handleForward() {
    if (isStepperMotorRunning) return; // Nu permite alte comenzi dacă motorul este activ
    isStepperMotorRunning = true;

    long steps = (220L * 2048L) / 360L; // 200 de grade -> pași
    stepper.setStepsToMake(steps, true);

    // Când motorul termină, setează isMotorRunning pe false
    while (stepper.isRunning()) {
        stepper.update();
        setNewStepperState();
        // delay(1); // Delay mic pentru a evita blocarea altor procese
        yield(); // Evită blocarea

    }

    isStepperMotorRunning = false; // Motorul a terminat execuția
}

void handleBackward() {
    if (isStepperMotorRunning) return; // Nu permite alte comenzi dacă motorul este activ
    isStepperMotorRunning = true;

    long steps = (210L * 2048L) / 360L; // 190 de grade -> pași
    stepper.setStepsToMake(steps, false);

    // Când motorul termină, setează isMotorRunning pe false
    while (stepper.isRunning()) {
        stepper.update();
        setNewStepperState();
        // delay(1); // Delay mic pentru a evita blocarea altor procese
        yield(); // Evită blocarea

    }

    isStepperMotorRunning = false; // Motorul a terminat execuția
}


unsigned long lastMotionTime = 0;  // Variabilă pentru a stoca timpul ultimei mișcări detectate
const unsigned long lightOnDuration = 5000;  // Durata în milisecunde (5 secunde)

void checkLightAndMotion() {
  ldrValue = digitalRead(LDR_PIN);
  pirValue = digitalRead(PIR_PIN);
  // Serial.print("PIR: ");
  // Serial.print(pirValue);
 
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

  // Serial.print("Umiditate: ");
  // Serial.print(h);
  // Serial.print("%  Temperatura: ");
  // Serial.print(t);
  // Serial.println("°C");
  
  // Trimite datele senzorilor la Firebase
  Firebase.setString(firebaseData, "/sensorData/temperature", t);
  Firebase.setString(firebaseData, "/sensorData/humidity", h);
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
String lastGarageLedState = "off";
String lastGuestLedState = "off";
String lastDoorLedState = "off";
String lastGuestDoorState = "closed";
String lastFrontDoorState = "closed";
String lastBedRoomDoorState = "closed";
String lastGuestWindowState = "closed";
String lastLivingWindowState = "closed";
String lastBedRoomWindowState = "closed";
String lastFanState = "stop";
String lastStatusMessage = "";

bool servoNeedsUpdate = false;

void streamCallback(MultiPathStreamData stream) {
    size_t numChild = sizeof(childPath) / sizeof(childPath[0]);

    // Iterăm prin fiecare copil din stream
    for (size_t i = 0; i < numChild; i++) {
        if (stream.get(childPath[i])) {
            String path = stream.dataPath;
            String value = stream.value.c_str();

            // Serial.printf("path: %s, value: %s\n", path.c_str(), value.c_str());

            // Gestionăm stările doar dacă s-au schimbat

            // Control LED-uri
            if (path == "/led/garageLed") {
                if (value != lastGarageLedState) {
                    lastGarageLedState = value;
                    value == "on" ? turnOnGarageLed() : turnOffGarageLed();
                }
            } else if (path == "/led/guestLed") {
                if (value != lastGuestLedState) {
                    lastGuestLedState = value;
                    value == "on" ? turnOnGuestLed() : turnOffGuestLed();
                }
            } else if (path == "/led/doorLed") {
                if (value != lastDoorLedState) {
                    lastDoorLedState = value;
                    value == "on" ? turnOnDoorLed() : turnOffDoorLed();
                }
            }

            // Control uși
            else if (path == "/door/guestDoor") {
                if (value != lastGuestDoorState) {
                    lastGuestDoorState = value;
                    value == "open" ? openGuestDoor() : closeGuestDoor();
                }
            } else if (path == "/door/frontDoor") {
                if (value != lastFrontDoorState) {
                    lastFrontDoorState = value;
                    value == "open" ? openFrontDoor() : closeFrontDoor();
                }
            } else if (path == "/door/bedRoomDoor") {
                if (value != lastBedRoomDoorState) {
                    lastBedRoomDoorState = value;
                    value == "open" ? openBedRoomDoor() : closeBedRoomDoor();
                }
            }

            // Control ferestre
            else if (path == "/window/guestWindow") {
                if (value != lastGuestWindowState) {
                    lastGuestWindowState = value;
                    value == "open" ? openGuestWindow() : closeGuestWindow();
                }
            } else if (path == "/window/livingWindow") {
                if (value != lastLivingWindowState) {
                    lastLivingWindowState = value;
                    value == "open" ? openLivingWindow() : closeLivingWindow();
                }
            } else if (path == "/window/bedRoomWindow") {
                if (value != lastBedRoomWindowState) {
                    lastBedRoomWindowState = value;
                    value == "open" ? openBedRoomWindow() : closeBedRoomWindow();
                }
            }

            // Control motor ventilator
            else if (path == "/motor/fan") {
                if (value != lastFanState) {
                    lastFanState = value;
                    value == "start" ? startFan() : stopFan();
                }
            }

            // Gestionăm mesaje de stare
            else if (path == "/status/message") {
                if (value != lastStatusMessage) {
                    lastStatusMessage = value;
                    Serial.println("Status message: " + value);
                }
            }
        }
    }

    Serial.printf("Received stream payload size: %d (Max. %d)\n\n", 
                  stream.payloadLength(), stream.maxPayloadLength());
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
  Serial.println("Motivul resetării: " + ESP.getResetReason());

    // Verificați dacă resetarea este cauzată de o excepție
    if (ESP.getResetReason() == "Exception") {
        Serial.println("Reset cauzat de excepție. Reinițializăm...");
    }
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

  
  // // Setarea buffer-ului pentru ESP8266
  fbdo.setBSSLBufferSize(4096, 1024);
  stream.setBSSLBufferSize(2048, 512);


// ///De aici apare lag-ul la componente, in special la motorul stepper
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Setarea MultiPath Stream
if (!Firebase.beginMultiPathStream(stream, parentPath)) {
    Serial.printf("Stream begin error, %s\n\n", stream.errorReason().c_str());
  }

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);

  Serial.println("Connected to Firebase");

  
///pana aici e lag

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
  server.on("/moveForward", handleForward);
  server.on("/moveBackward", handleBackward);
  

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

unsigned long lastFirebaseTime = 0;
const unsigned long firebaseInterval = 200; // 2 secunde


void loop() {
  // Gestionare server și alte funcții
  // ESP.wdtFeed(); // Resetează watchdog-ul (am inteles ca e pt buclele lungi ca sa nu se mai dea reset)

  server.handleClient();
  // updateServoPWM(); 
  //  if (servoNeedsUpdate) {
  //       updateServoPWM();
  //       servoNeedsUpdate = false; // Resetează flag-ul după actualizare
  //   }
 stepper.update();
  if (stepper.isUpdated) {
    setNewStepperState();
    stepper.clearUpdated();
  } 

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
}

