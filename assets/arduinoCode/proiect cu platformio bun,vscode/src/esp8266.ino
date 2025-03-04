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

#include <ArduinoOTA.h> //pentru programare prin wifi



// WiFi credentials
const char* ssid = "Seco";
const char* password = "secosanpq1";


// WiFi credentials
// const char* ssid = "xSecoI";
// const char* password = "seco1212";

// Firebase configuration
#define FIREBASE_HOST "licentalivedb-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "K9re6wdo8C5r860LD0TnPoSAo2I96n1NgMuFP3wJ"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;


// Create a web server object
ESP8266WebServer server(80);


#define LDR_PIN D0  
#define DHT_PIN D1  
#define PIR_PIN D2  


// Pins for 74HC595
const int dataPin = D5;
const int clockPin = D6;
const int latchPin = D8;

// Create ShiftRegister74HC595 object with 2 shift registers
ShiftRegister74HC595<3> sr(dataPin, clockPin, latchPin);


// PWM dimming on TX and RX
#define TX_GUEST_LED 1  // GPIO1
#define RX_BEDROOM_LED 3  // GPIO3
//Led
#define GARAGE_LEFT_LED 10
#define GARAGE_RIGHT_LED 11
#define DOOR_LEFT_LED 13
#define DOOR_RIGHT_LED 12
#define HALL_LED 16
#define LIVING_LED1 17
#define LIVING_LED2 18

//Fan
#define MOTOR_IN1 19  
#define MOTOR_IN2 20  
#define MOTOR_ENABLE 21 

// Define channels for the doors with servo motor on the first shift register
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

// Buzzer
#define BUZZER_PIN 22

// Motor pas cu pas cu 2048 pași
BiPolStepper stepper(2048); 


//Sensors
#define DHTTYPE DHT11 //Temperatura si umiditate
DHT dht(DHT_PIN, DHTTYPE);
#define RAIN_SENSOR_PIN D3
#define GAS_SENSOR_PIN A0
#define FIRE_SENSOR_PIN D4
#define MAGNETICREDSWITCH_PIN D7

// Variables for sensors
int ldrValue = 0; //Senzor de lumina, light dependent resistor
int pirValue = 0; //Senzor de miscare, passive infrared sensor

// Stepper motor variables
volatile bool isStepperMotorRunning = false; 
volatile bool isRunning = false;

//Variable for servo motor
// const int pulseMin = 544; // Minimum pulse width in microseconds
// const int pulseMax = 2400; // Maximum pulse width in microseconds

// Variabilă pentru starea geamurilor
bool windowsClosed = false; 

// Variabilă pentru controlul manual al LED-ului
bool manualControlHallLed = false;

// Variabilă pentru starea de securitate a casei
bool isHouseArmed = false;



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


//Doar pentru HTML
void handleSensorsStatus() {
  String message = "Status: ";
  
  int fireState = digitalRead(FIRE_SENSOR_PIN);
  int gasState = analogRead(GAS_SENSOR_PIN);
  int rainValue = digitalRead(RAIN_SENSOR_PIN);
  float freeRam = ESP.getFreeHeap() / 1024.0;  // Convertim bytes în kilobytes

  if (fireState == LOW) {
    message += "Fire detected! ";

  } else {
    message += "No fire. ";
  }

  message += "Gas Value: " + String(gasState) + "\n";
  message += "Rain Value: " + String(rainValue) + "\n";
  message += "RAM: " + String(freeRam, 2) + " kB" + "\n";


  server.send(200, "text/plain", message);
}

void handleMotionStatus() {
  String message = "Status: ";
  
  int pirValue = digitalRead(PIR_PIN);
  int reedSwitchState = digitalRead(MAGNETICREDSWITCH_PIN); 

  message += "Door Status: " + String(reedSwitchState) + "\n";

  if (pirValue == HIGH) {
    message += "Motion Detected!";
  } else {
    message += "No Motion. ";
  }
  server.send(200, "text/plain", message);
}


void checkRain() {
    static bool lastRainDetected = false;

    bool currentRainDetected = (digitalRead(RAIN_SENSOR_PIN) == LOW); // LOW semnifică ploaie

    if (currentRainDetected != lastRainDetected) {
        String rainStatus = currentRainDetected ? "It's Raining!" : "No Rain";
        Firebase.setString(firebaseData, "/sensorData/rain", rainStatus);
        lastRainDetected = currentRainDetected;
    }

    if (currentRainDetected) {
        closeWindows();
    }
}


void closeWindows() {
    // Serial.println("Closing windows...");
    closeGuestWindow();
    closeLivingWindow();
    closeBedRoomWindow();
}

void checkFire() {
    static bool lastFireDetected = false;
    bool currentFireDetected = (digitalRead(FIRE_SENSOR_PIN) == LOW);

    if (currentFireDetected != lastFireDetected) {
        String fireStatus = currentFireDetected ? "Fire!" : "No Fire";
        Firebase.setString(firebaseData, "/sensorData/fire", fireStatus);
        lastFireDetected = currentFireDetected;

        if (currentFireDetected) {
            activateBuzzer();
        } else {
            deactivateBuzzer();
        }
    }     
}


void checkGas() {
    static bool lastGasDetected = false;
    int gasValue = analogRead(GAS_SENSOR_PIN); 
    bool currentGasDetected = (gasValue > 1200); 

    if (currentGasDetected != lastGasDetected) {
        String gasStatus = currentGasDetected ? "Gas!" : "No Gas";
        Firebase.setString(firebaseData, "/sensorData/gas", gasStatus);
        Firebase.setString(firebaseData, "/sensorData/gasValue", gasValue);
        lastGasDetected = currentGasDetected;

        if (currentGasDetected) {
            activateBuzzer();
        } else {
            deactivateBuzzer();
        }
    }
      Firebase.setString(firebaseData, "/sensorData/gasValue", gasValue);
}

// Functions to handle LED control
void turnOnGarageLed() {
  sr.set(GARAGE_LEFT_LED, true);
  sr.set(GARAGE_RIGHT_LED, true);
}

void turnOffGarageLed() {
  sr.set(GARAGE_LEFT_LED, false);
  sr.set(GARAGE_RIGHT_LED, false);
}

void turnOnGuestLed() {
  digitalWrite(TX_GUEST_LED, HIGH); 
}

void turnOffGuestLed() {
  digitalWrite(TX_GUEST_LED, LOW); 
}

void turnOnDoorLed() {
  sr.set(DOOR_LEFT_LED, true);
  sr.set(DOOR_RIGHT_LED, true);
}

void turnOffDoorLed() {
  sr.set(DOOR_LEFT_LED, false);
  sr.set(DOOR_RIGHT_LED, false);
}

void turnOnBedroomLed() {
  digitalWrite(RX_BEDROOM_LED, HIGH); 
}

void turnOffBedroomLed() {
  digitalWrite(RX_BEDROOM_LED, LOW); 
}

void turnOnHallLed() {
  sr.set(HALL_LED, true);
}

void turnOffHallLed() {
  sr.set(HALL_LED, false);
}

void turnOnLivingLed1() {
  sr.set(LIVING_LED1, true);
  sr.set(LIVING_LED2, true);
}

void turnOffLivingLed1() {
  sr.set(LIVING_LED1, false);
  sr.set(LIVING_LED2, false); 
}

void turnOnLivingLed2() {
  sr.set(LIVING_LED2, true);
}

void turnOffLivingLed2() {
  sr.set(LIVING_LED2, false); 
}


    //Doors
//GuestDoor

void openGuestDoor() {
    setServoPosition(GUEST_DOOR, 158);
}

void closeGuestDoor() {
    setServoPosition(GUEST_DOOR, 24);
}


//FrontDoor
void openFrontDoor() {
  setServoPosition(FRONT_DOOR, 135); 
}

void closeFrontDoor() {
  setServoPosition(FRONT_DOOR, 31); 
}

//BedRoomDoor
void openBedRoomDoor() {
  setServoPosition(BED_ROOM_DOOR, 28);
}

void closeBedRoomDoor() {
  setServoPosition(BED_ROOM_DOOR, 165);
}


  //Windows
void openGuestWindow() {
  setServoPosition(GUEST_WINDOW, 0);
}

void closeGuestWindow() {
    setServoPosition(GUEST_WINDOW, 80);
}


void openBedRoomWindow() {
    setServoPosition(BED_ROOM_WINDOW, 130);
}

void closeBedRoomWindow() {
    setServoPosition(BED_ROOM_WINDOW, 48);
}


void openLivingWindow() {
    setServoPosition(LIVING_WINDOW, 120);
}

void closeLivingWindow() {
    setServoPosition(LIVING_WINDOW, 25); 
}


const unsigned long pwmPeriod = 20000; // 20ms în microsecunde
const int pulseMin = 400;  
const int pulseMax = 2600;


void setServoPosition(int pin, int position) {
    int pulseWidth = map(position, 0, 180, pulseMin, pulseMax); //transforma din pozitia pe care o primeste(0-180) in ce avem nevoie in functie de puls, ex pentru 90 de grade avem nevoie de 1500 de microsecunde
    // Serial.print("Setare servo poziție: ");
    // Serial.println(position);

    // Generăm semnal PWM timp de 1 secundă (50 cicluri la 20 ms fiecare pentru a fi sigur ca s-a trimis informatia, poate fi doar o data dar nu mereu ia din prima motorul)
    for (int i = 0; i < 50; i++) {
        sr.set(pin, HIGH);  // Pin HIGH pentru durata impulsului
        delayMicroseconds(pulseWidth); //menține pinul HIGH pentru durata impulsului PWM corespunzătoare poziției dorite a servo-ului. Dacă poziția este 90 de grade, pulseWidth va fi 1500 µs, deci pinul rămâne HIGH timp de 1500 µs.
        sr.set(pin, LOW);   // Pin LOW pentru restul perioadei
        delayMicroseconds(pwmPeriod - pulseWidth); //Aceasta linie menține pinul LOW pentru restul perioadei de 20 ms (20000 µs). Deoarece un ciclu PWM complet trebuie să fie de 20 ms, scădem durata impulsului (pulseWidth) pentru a menține frecvența corectă. De exemplu, pentru un pulseWidth de 1500 µs, pinul rămâne LOW pentru 20000 - 1500 = 18500 µs.
        yield(); // Previne resetarea watchdog-ului
    }
}

// Store brightness (0-100 range)
int brightnessTX = 0;
int brightnessRX = 0;

void handleLedWithPWM(int pin, int brightnessPercentage) {
    if (brightnessPercentage == 0) {
        digitalWrite(pin, LOW); 
    } else if (brightnessPercentage == 100) {
        digitalWrite(pin, HIGH); 
    } else {
        analogWrite(pin, brightnessPercentage); 
    }
}


//Stepper
void setNewStepperState() {
  // Actualizăm starea pinii IN1, IN2, IN3 și IN4
  sr.set(IN1, stepper.state[0]);
  sr.set(IN2, stepper.state[1]);
  sr.set(IN3, stepper.state[2]);
  sr.set(IN4, stepper.state[3]);
}


void handleForward() {
  if (isStepperMotorRunning) {
    server.send(200, "text/plain", "Motor is already running");
    return; // Nu permite alte comenzi dacă motorul este activ
}    
    isStepperMotorRunning = true;

    // long steps = (205L * 2048L) / 360L; // 205 de grade -> pași; 2048L reprezintă numărul total de pași pentru o rotație completă (360 de grade) a motorului pas cu pas.
    // long steps = (178L * 2048L) / 360L; // 205 de grade -> pași; 2048L reprezintă numărul total de pași pentru o rotație completă (360 de grade) a motorului pas cu pas.

    long steps = (175L * 2048L) / 360L; // 205 de grade -> pași; 2048L reprezintă numărul total de pași pentru o rotație completă (360 de grade) a motorului pas cu pas.
    stepper.setStepsToMake(steps, true); //Se setează motorul să facă un anumit număr de pași. Parametrul 2 indică direcția (true pentru înainte, false pentru înapoi).

    // Când motorul termină, setează isMotorRunning pe false, e nevoie de while pentru ca setStepsToMake setează numărul de pași și direcția, dar NU îi execută imediat, actualizarea motorului se face prin update() care trebuie apelata periodic pentru a continua miscarea. update() verifică dacă a trecut suficient timp (STEP_TIME) și apoi face următorul pas.
    //Ține codul în buclă până când motorul termină toți pașii. Apelează stepper.update(); continuu, astfel încât motorul să continue să se rotească.

    while (stepper.isRunning()) {
        stepper.update();
        setNewStepperState();
        yield(); // Permite altor procese să ruleze
    }
    
    isStepperMotorRunning = false; // Motorul a terminat execuția

    //  if (!stepper.isRunning()) {
    //   deactivateStepper();
    // }
    deactivateStepper();

    server.send(200, "text/plain", "Motor moved  190 degrees");
}

void handleBackward() {
  if (isStepperMotorRunning) {
    server.send(200, "text/plain", "Motor is already running");
    return; // Nu permite alte comenzi dacă motorul este activ
} 
   isStepperMotorRunning = true;

    // long steps = (190L * 2048L) / 360L; // 190 de grade -> pași
    long steps = (175L * 2048L) / 360L; // 190 de grade -> pași

    stepper.setStepsToMake(steps, false);

    // Când motorul termină, setează isMotorRunning pe false
    while (stepper.isRunning()) {
        stepper.update();
        setNewStepperState();
        yield(); // Permite altor procese să ruleze
    }

    // deactivateStepper();
    isStepperMotorRunning = false; // Motorul a terminat execuția
    deactivateStepper();

    //  if (!stepper.isRunning()) {
    //   deactivateStepper();
    // }
    server.send(200, "text/plain", "Motor moved backward 190 degrees");
}

void handleCustomDegrees(int degrees) {
  if (isStepperMotorRunning) return; // Nu permite alte comenzi dacă motorul este activ
  isStepperMotorRunning = true;

  // Determinăm direcția în funcție de semnul gradelor
  bool direction = (degrees >= 0); // true pentru grade pozitive, false pentru grade negative

  // Calculăm numărul de pași necesari pentru gradele introduse (folosim valoarea absolută)
  long steps = (abs(degrees) * 2048L) / 360L; // 2048L reprezintă numărul total de pași pentru o rotație completă (360 de grade)

  stepper.setStepsToMake(steps, direction); // Setăm motorul să facă numărul de pași calculat, cu direcția corespunzătoare

  // Buclă pentru a executa pașii motorului
  while (stepper.isRunning()) {
      stepper.update();
      setNewStepperState();
      yield(); // Permite altor procese să ruleze
  }

  isStepperMotorRunning = false; // Motorul a terminat execuția

  if (!stepper.isRunning()) {
      deactivateStepper();
  }
}


void deactivateStepper() {
  sr.set(IN1, LOW);
  sr.set(IN2, LOW);
  sr.set(IN3, LOW);
  sr.set(IN4, LOW);
}


unsigned long lastMotionTime = 0;  // Variabilă pentru a stoca timpul ultimei mișcări detectate
const unsigned long lightOnDuration = 5000;  // Durata în milisecunde (5 secunde)

void checkMotion(){
  bool currentMotionDetected = (digitalRead(PIR_PIN) == HIGH);
  static bool lastMotionDetected = false;

   if (currentMotionDetected != lastMotionDetected) {
        String motionStatus = currentMotionDetected ? "Motion Detected!" : "No Motion";
        Firebase.setString(firebaseData, "/sensorData/motion", motionStatus);
        lastMotionDetected = currentMotionDetected;
        // Serial.println("Actualizat starea senzorului de mișcare în Firebase.");
    }
}

void checkLightAndMotion() {
    static bool lastIsNight = false;
    static bool lastManualControlHallLed = false;

    bool currentIsNight = (digitalRead(LDR_PIN) == HIGH);
    bool currentMotionDetected = (digitalRead(PIR_PIN) == HIGH);

    if (currentIsNight != lastIsNight) {
      String lightStatus = "";
      if (currentIsNight) {
            manualControlHallLed = false;
            Firebase.setString(firebaseData, "/led/hallLedStatus", "auto");
            lightStatus = "Night";
        } else {
            // Dacă LED-ul era controlat automat și este încă aprins, îl stingem acum
            turnOffHallLed(); 
            Firebase.setString(firebaseData, "/led/hallLedStatus", "manual");
            lightStatus = "Day";
            manualControlHallLed = true;
        }

        Firebase.setString(firebaseData, "/sensorData/isDay", lightStatus);
        lastIsNight = currentIsNight;
        // Serial.println("Actualizat starea de lumină (zi/noapte) în Firebase.");
    }

    if (currentMotionDetected && currentIsNight && !manualControlHallLed) {
        turnOnHallLed();
        lastMotionTime = millis();  
    }

    // Dacă e noapte, nu e mișcare și nu e modul manual, stingem LED-ul după o perioadă
    if (!currentMotionDetected && currentIsNight && !manualControlHallLed && millis() - lastMotionTime > lightOnDuration) {
        turnOffHallLed();
    }
}


// Funcție pentru a reveni la controlul automat după un anumit timp (opțional)
void resetManualControl() {
    manualControlHallLed = false;
}

void  displayTemperatureHumidity() {
    static float lastTemperature = -999; // Valoare imposibilă pentru inițializare
    static float lastHumidity = -999; // Valoare imposibilă pentru inițializare

    float currentTemperature = dht.readTemperature();
    float currentHumidity = dht.readHumidity();
    controlFanBasedOnTemperature(currentTemperature);

    // Verificăm dacă citirea senzorului este validă
    if (isnan(currentTemperature) || isnan(currentHumidity)) {
        // Serial.println("Eroare la citirea senzorului DHT!");
        Firebase.setString(firebaseData, "/sensorData/temperature", "Eroare la citirea senzorului DHT!");
        return;
    }

    // Actualizează Firebase doar dacă valorile s-au schimbat
    if (currentTemperature != lastTemperature) {
        Firebase.setFloat(firebaseData, "/sensorData/temperature", currentTemperature);
        lastTemperature = currentTemperature;
    }

    if (currentHumidity != lastHumidity) {
        Firebase.setFloat(firebaseData, "/sensorData/humidity", currentHumidity);
        lastHumidity = currentHumidity;
    }
}

//Fan
void startFan() {
  sr.set(MOTOR_ENABLE, HIGH); //Controleaza daca motorul primeste sau nu curent
  sr.set(MOTOR_IN1, HIGH);  // Setează IN1 la HIGH (direcția înainte)
  sr.set(MOTOR_IN2, LOW);   // Setează IN2 la LOW
}


void stopFan() {
  // Setează pinii de direcție și pinul enable la LOW pentru a opri motorul
  sr.set(MOTOR_IN1, LOW);
  sr.set(MOTOR_IN2, LOW);
  sr.set(MOTOR_ENABLE, LOW);  //Nu mai primeste curent motorul
}


// Variabile globale
String fanStatus = "manual"; 
String fanCommand = "stop"; 
int temperatureThreshold = 35; // Prag implicit pentru temperatura din Firebase 

// Functia pentru controlul ventilatorului pe baza temperaturii
void controlFanBasedOnTemperature(int currentTemperature) {
    if (fanStatus == "auto") {
        if (currentTemperature >= temperatureThreshold) {
            startFan();
        } else {
            stopFan();
        }
    } else if (fanStatus == "manual") {
        if (fanCommand == "start") {
            startFan();
        } else if (fanCommand == "stop") {
            stopFan();
        }
    }
}

 // Variabilă globală pentru a menține starea alarmei
bool isAlarmTriggered = false;

// Funcție pentru a arma casa
void armHouse() {
    Firebase.setString(firebaseData, "/security/status", "Armed");

    // Verificăm dacă există mișcare sau ușa este deschisă
    bool currentMotionDetected = (digitalRead(PIR_PIN) == HIGH);
    bool isDoorOpen = (digitalRead(MAGNETICREDSWITCH_PIN) == HIGH); // Ușa este deschisă dacă starea este HIGH

    if (currentMotionDetected || isDoorOpen) {
        if (!isAlarmTriggered) {
            activateBuzzer(); 
            String alertMessage = "Alarm!!";
            if (currentMotionDetected) {
                alertMessage += " Motion detected!";
            }
            if (isDoorOpen) {
                alertMessage += " The door is open!";
            }
            Firebase.setString(firebaseData, "/security/alert", alertMessage);
            isAlarmTriggered = true; // Setăm starea alarmei ca fiind declanșată
        }
    }
}


void disarmHouse() {
    isHouseArmed = false; 
    isAlarmTriggered = false; 
    deactivateBuzzer(); 
    resetManualControl(); 
    Firebase.setString(firebaseData, "/security/status", "Disarmed");
    Firebase.setString(firebaseData, "/security/alert", "Disarmed");
}


void showWiFiSignalStrength() {
  int rssi = WiFi.RSSI(); // valori intre -100 si 0 unde 0 este cel mai bun semnal
  Firebase.setString(firebaseData, "/statusESP8266/wifiSignal", 
      "RSSI: " + String(rssi) + " dBm");
}

void showFreeHeap() {
  static String lastRamStatus = "";

  // Obținem memoria liberă și totală direct în kilobytes
  float freeRam = ESP.getFreeHeap() / 1024.0;  // Convertim bytes în kilobytes
  float totalRam = 50000.0 / 1024.0;           // Total RAM în kilobytes

  // Calculăm procentajul memoriei libere
  float freePercentage = (freeRam / totalRam) * 100.0;
  String currentRamStatus = String(freeRam, 2) + " kB (" + String(freePercentage, 2) + "%)";

  if (currentRamStatus != lastRamStatus) {
      // Trimitem totalul și memoria liberă în kilobytes
      Firebase.setString(firebaseData, "/statusESP8266/ram/totalRAM", String(totalRam, 2) + " kB");
      Firebase.setString(firebaseData, "/statusESP8266/ram/freeRAM", String(freeRam, 2) + " kB");
      Firebase.setString(firebaseData, "/statusESP8266/ram/percentRAM",  String(freePercentage, 2) + " %");

      // Salvăm starea pentru actualizări viitoare
      lastRamStatus = currentRamStatus;
      // Serial.println("Actualizat starea RAM-ului în Firebase.");
  }
}


void reportStaticInfo() {
  //Adresa IP
  Firebase.setString(firebaseData, "/statusESP8266/IP", WiFi.localIP().toString());

  // Adresa MAC
  Firebase.setString(firebaseData, "/statusESP8266/macAddress", WiFi.macAddress());

  // Frecvența CPU
  Firebase.setString(firebaseData, "/statusESP8266/cpuFreq", String(ESP.getCpuFreqMHz()) + " MHz");

  // // Dimensiunea totală Flash
  // Firebase.setString(firebaseData, "/statusESP8266/flash/totalSize", String(ESP.getFlashChipSize()) + " bytes");

  // // Dimensiunea libera Flash
      // Firebase.setString(firebaseData, "/statusESP8266/flash/freeSize", String(ESP.getFreeSketchSpace()) + " bytes");
  // Dimensiunea totală Flash în MB

  Firebase.setString(firebaseData, "/statusESP8266/flash/totalSize", 
  String(ESP.getFlashChipSize() / 1048576.0, 2) + " MB");

  // Dimensiunea liberă Flash în MB
  Firebase.setString(firebaseData, "/statusESP8266/flash/freeSize", 
  String(ESP.getFreeSketchSpace() / 1048576.0, 2) + " MB");

  // Versiunea SDK
  Firebase.setString(firebaseData, "/statusESP8266/sdkVersion", ESP.getSdkVersion());

}

void handleReset() {
  ESP.restart();
}

void magneticReedSwitch() { //Funcție pentru a verifica starea senzorului de ușă
  static int lastReedSwitchState = -1; // Variabilă pentru stocarea stării anterioare

  int ReedSwitchState = digitalRead(MAGNETICREDSWITCH_PIN); 
  if (ReedSwitchState != lastReedSwitchState) { // Actualizăm Firebase doar dacă starea s-a schimbat
    String doorStatus = (ReedSwitchState == LOW) ? "Closed" : "Opened";
    Firebase.setString(firebaseData, "/sensorData/doorStatus", doorStatus);
    lastReedSwitchState = ReedSwitchState; // Actualizăm starea anterioară
  }
}


void activateBuzzer() {
sr.set(BUZZER_PIN, HIGH); 
}

void deactivateBuzzer() {
sr.set(BUZZER_PIN, LOW); 
}


String lastGarageDoorState = "open"; // Starea inițială presupusă

FirebaseData fbdo;   // Obiect utilizat pentru operațiuni standard cu Firebase (scriere/citire de date)
FirebaseData stream; // Obiect utilizat pentru a asculta în timp real modificările din Firebase (streaming de date)

String parentPath = "/";  
String childPath[21] = {
  "/led/guest/ledDim",
  "/led/bedRoom/ledDim",
  "/led/guest/led",
  "/led/bedRoom/led",
  "/led/garageLed",
  "/led/doorLed",
  "/led/hallLed",
  "/led/hallLedStatus",
  "/led/livingLed",
  "/door/guestDoor",
  "/door/frontDoor",
  "/door/bedRoomDoor",
  "/door/garage",
  "/window/guestWindow",
  "/window/livingWindow",
  "/window/bedRoomWindow",
  "/motor/fan",
  "/motor/fanStatus",
  "/motor/temperatureForStartingFan",
  "/statusESP8266/reset",
  "/security",
};


void streamCallback(MultiPathStreamData stream) {
    // Dacă motorul este activ, ignorăm toate celelalte comenzi
    if (isStepperMotorRunning) {
        // Serial.println("Motorul este activ. Aștept terminarea execuției...");
        return;
    }

    size_t numChild = sizeof(childPath) / sizeof(childPath[0]);
    for (size_t i = 0; i < numChild; i++) {
        if (stream.get(childPath[i])) {
            String path = stream.dataPath;
            String value = stream.value.c_str();
            int brightnessPercentage = value.toInt(); // Convert the value from string to integer

            // Handle led path
            if (path == "/led/guest/ledDim") {
                handleLedWithPWM(TX_GUEST_LED, brightnessPercentage); 
                brightnessTX = brightnessPercentage;
            } else if (path == "/led/bedRoom/ledDim") {
                handleLedWithPWM(RX_BEDROOM_LED, brightnessPercentage); 
                brightnessRX = brightnessPercentage;
            } else if (path == "/led/garageLed") {
                value == "on" ? turnOnGarageLed() : turnOffGarageLed();
            } else if (path == "/led/doorLed") {
                value == "on" ? turnOnDoorLed() : turnOffDoorLed();
            } else if (path == "/led/hallLed") {
                if (value == "on") {
                    turnOnHallLed();
                    manualControlHallLed = true; 
                    Firebase.setString(firebaseData, "/led/hallLedStatus", "manual");
                } else if (value == "off") {
                    turnOffHallLed();
                    manualControlHallLed = true; 
                    Firebase.setString(firebaseData, "/led/hallLedStatus", "manual");
                 } 
              }else if (path == "/led/hallLedStatus") {
                if (value == "auto") {
                    manualControlHallLed = false; // Permitem controlul automat
                   }
                else if (value == "manual"){
                      manualControlHallLed = true; 

                }
          } else if (path == "/led/livingLed") {
            if (value == "on") {
              turnOnLivingLed1();
            } else {
              turnOffLivingLed1();
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
              // Gestionăm comanda doar dacă valoarea s-a schimbat
              if (value != lastGarageDoorState) {
                lastGarageDoorState = value;
                if (value == "open") {
                  handleBackward();  
                } else if (value == "close") {
                  handleForward();
                } 
              } 
              // Odată ce comanda pentru stepper este procesată, ieșim din callback
              return;
          }

          // Control ferestre
          else if (path == "/window/guestWindow") {
              value == "open" ? openGuestWindow() : closeGuestWindow();
          } else if (path == "/window/livingWindow") {
              value == "open" ? openLivingWindow() : closeLivingWindow();
          } else if (path == "/window/bedRoomWindow") {
              value == "open" ? openBedRoomWindow() : closeBedRoomWindow();
          }

          // Control ventilator
          else if (path == "/motor/fan") {
            fanCommand = value;
            if (fanStatus == "manual") {
              fanCommand == "start" ? startFan() : stopFan();
            }
          } else if (path == "/motor/fanStatus") {
              fanStatus = value;
          } else if (path == "/motor/temperatureForStartingFan") {
              temperatureThreshold = value.toInt();
          }

            // Comenzi pentru armarea sau dezarmarea casei
          else if (path == "/security/arm") {
              if (value == "arm") {
                isHouseArmed = true; 
                Firebase.setString(firebaseData, "/security/alert", "Clear");
              } else if (value == "disarm") {
                disarmHouse(); 
            }
          }
          
          //Resetare microcontroller
          else if (path == "/statusESP8266/reset") {
            if (value == "reset") {
              handleReset();
            }    
          }        
        }
    }
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
//Initializare senzori si componente
  void setup() {
  Serial.begin(115200);
  Serial.println("Motivul resetării: " + ESP.getResetReason());

  dht.begin();
  delay(500);
  sr.setAllLow();

  //Configurare WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to " + String(ssid));
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed!");
  }

   // Configurare Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // stream.setBSSLBufferSize(512, 128); 
  // stream.setBSSLBufferSize(1024, 256); //e bun in caz ca nu merge standard
  // stream.setBSSLBufferSize(2048, 256);//poate mai merge 

  // Setarea MultiPath Stream
  if (!Firebase.beginMultiPathStream(stream, parentPath)) {
    Serial.printf("Stream begin error, %s\n\n", stream.errorReason().c_str());
  }

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);
  Firebase.setString(firebaseData, "/statusESP8266/reset", "no reset");
  // Serial.println("Connected to Firebase");

  reportResetReason();
  reportStaticInfo();

  // Configurare pini
  configurePins();

  // Configurare server web
  setupWebRoutes();
  server.begin();
  Serial.println("HTTP server started");

  // Configurare OTA, programare prin wi-fi
  setupOTA();
}

void configurePins(){
   pinMode(TX_GUEST_LED, OUTPUT);
   pinMode(RX_BEDROOM_LED, OUTPUT);
   pinMode(LDR_PIN, INPUT);
   pinMode(PIR_PIN, INPUT);
   pinMode(RAIN_SENSOR_PIN, INPUT);
   pinMode(MAGNETICREDSWITCH_PIN,INPUT_PULLUP); 
   pinMode(FIRE_SENSOR_PIN, INPUT);
   pinMode(GAS_SENSOR_PIN, INPUT);
}

void setupOTA() {
   // Nume device (opțional)
   ArduinoOTA.setHostname("esp8266_Raul");
   // Parolă OTA (opțional):
  //  ArduinoOTA.setPassword("raul");
   ArduinoOTA.begin();
}

void setupWebRoutes() {
  server.on("/", handleRoot);
  server.on("/temperature_humidity", handleTemperatureHumidity);
  server.on("/sensors_status", handleSensorsStatus);
  server.on("/motion_status", handleMotionStatus);
  server.on("/moveCustomDegrees", handleMoveCustomDegrees); 

  setupBuzzerRoutes();
  setupLedRoutes();
  setupDoorRoutes();
  setupWindowRoutes();
  setupMotorRoutes();
}

void setupBuzzerRoutes() {
  server.on("/onBuzzer", activateBuzzer);
  server.on("/OffBuzzer", deactivateBuzzer);
}

void handleMoveCustomDegrees() {
  if (server.hasArg("degrees")) {
      int degrees = server.arg("degrees").toInt();
      handleCustomDegrees(degrees);
      server.send(200, "text/plain", "Moving " + String(degrees) + " degrees");
  } else {
      server.send(400, "text/plain", "Degrees parameter missing");
  }
}

void setupLedRoutes() {
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
  server.on("/livingLedOn", []() { turnOnLivingLed1(); turnOnLivingLed2(); });
  server.on("/livingLedOff", []() { turnOffLivingLed1(); turnOffLivingLed2(); });
}

void setupDoorRoutes() {
  server.on("/openGuestDoor", openGuestDoor);
  server.on("/closeGuestDoor", closeGuestDoor);
  server.on("/openFrontDoor", openFrontDoor);
  server.on("/closeFrontDoor", closeFrontDoor);
  server.on("/openBedRoomDoor", openBedRoomDoor);
  server.on("/closeBedRoomDoor", closeBedRoomDoor);
}

void setupWindowRoutes() {
  server.on("/openGuestWindow", openGuestWindow);
  server.on("/closeGuestWindow", closeGuestWindow);
  server.on("/openLivingWindow", openLivingWindow);
  server.on("/closeLivingWindow", closeLivingWindow);
  server.on("/openBedRoomWindow", openBedRoomWindow);
  server.on("/closeBedRoomWindow", closeBedRoomWindow);
}

void setupMotorRoutes() {
  server.on("/moveForward", handleForward);
  server.on("/moveBackward", handleBackward);
  server.on("/startFan", startFan);
  server.on("/stopFan", stopFan);
}


// Main loop
// Timere pentru senzori 
unsigned long lastMagneticReedSwitchReadTime = 0;
const unsigned long magneticReedSwitchSensorInterval = 2000; // 2 secunde

unsigned long lastSensorReadTime = 0;
const unsigned long sensorReadInterval = 5000; // 5 secunde

void loop() {
  server.handleClient();

  unsigned long currentTime = millis();
  // Citirea senzorilor la intervalul definit
   if (currentTime - lastMagneticReedSwitchReadTime >= magneticReedSwitchSensorInterval) {
    lastMagneticReedSwitchReadTime = currentTime;
    magneticReedSwitch();
    checkMotion();
  }

  if (currentTime - lastSensorReadTime >= sensorReadInterval) {
    lastSensorReadTime = currentTime;
    readAllSensors();
  }

  // Tratare cereri OTA
  ArduinoOTA.handle();
}

void readAllSensors() {
    checkLightAndMotion();
    displayTemperatureHumidity();
    checkFire();
    checkGas();
    checkRain();
    showFreeHeap();  
    showWiFiSignalStrength();
    if (isHouseArmed) {
      armHouse();
    }
}

void reportResetReason() {
    String resetReason = ESP.getResetReason();

    // // Afișăm motivul resetării în Serial Monitor
    // Serial.print("Motivul resetării: ");
    // Serial.println(resetReason);
     Firebase.setString(firebaseData, "/statusESP8266/resetReason", resetReason);
}