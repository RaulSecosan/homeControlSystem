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
#include <Ticker.h>

Ticker toneTicker;


// WiFi credentials
const char* ssid = "Seco";
const char* password = "secosanpq1";

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
// #define GUEST_LED   12
#define DOOR_LEFT_LED 13
#define DOOR_RIGHT_LED 14
// #define BEDROOM_LED 15
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

// Buzzer
#define BUZZER_PIN 22


BiPolStepper stepper(2048); // Motor pas cu pas cu 2048 pași


//Temperatura
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);


#define RAIN_SENSOR_PIN D3
#define GAS_SENSOR_PIN A0
#define FIRE_SENSOR_PIN D4

#define HALL_SENSOR_PIN D7

int ldrValue = 0;
int pirValue = 0;
volatile bool isStepperMotorRunning = false; 


// Stepper motor 
volatile bool isRunning = false;

// Variables for servo control
int guestDoorPosition; 
int frontDoorPosition;
int bedRoomDoorPosition; 
int guestWindowPosition; 
int bedRoomWindowPosition; 
int livingWindowPosition; 

// const int pulseMin = 544; // Minimum pulse width in microseconds
// const int pulseMax = 2400; // Maximum pulse width in microseconds



bool windowsClosed = false; // Variabilă pentru starea geamurilor






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
    static bool lastFireDetected = false;

    bool currentFireDetected = (digitalRead(FIRE_SENSOR_PIN) == LOW);

    if (currentFireDetected != lastFireDetected) {
        String fireStatus = currentFireDetected ? "Fire Detected!" : "No Fire";
        Firebase.setString(firebaseData, "/sensorData/fire", fireStatus);
        lastFireDetected = currentFireDetected;
        activateBuzzer();
        // Serial.println("Actualizat starea senzorului de foc în Firebase.");
    }
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



void checkGas() {
    static bool lastGasDetected = false;

    int gasValue = analogRead(GAS_SENSOR_PIN); 
    bool currentGasDetected = (gasValue > 1000); 

    if (currentGasDetected != lastGasDetected) {
        String gasStatus = currentGasDetected ? "Gas Detected!" : "No Gas";
        Firebase.setString(firebaseData, "/sensorData/gas", gasStatus);
        Firebase.setString(firebaseData, "/sensorData/gasValue", gasValue);

        lastGasDetected = currentGasDetected;

        if (currentGasDetected) {
            activateBuzzer(); 
            // Serial.println("Buzzzz Gas");
            
        }
    }
}



void closeWindows() {
    // Serial.println("Closing windows...");
    closeGuestWindow();
    closeLivingWindow();
    closeBedRoomWindow();
}

//Doar pentru HTML
void handleSensorsStatus() {
  String message = "Status: ";
  
  int fireState = digitalRead(FIRE_SENSOR_PIN);
  int gasState = analogRead(GAS_SENSOR_PIN);
  int rainValue = digitalRead(RAIN_SENSOR_PIN);
  int pirValue = digitalRead(PIR_PIN);
  int hallState = digitalRead(HALL_SENSOR_PIN); 

  if (fireState == LOW) {
    message += "Fire detected! ";

  } else {
    message += "No fire. ";
  }

  message += "Gas Value: " + String(gasState) + "\n";
  message += "Rain Value: " + String(rainValue) + "\n";
  message += "Door Status: " + String(hallState) + "\n";

  // if (gasState == LOW) {
  //   message += "Gas detected! ";

  // } else {
  //   message += "No gas. ";

  // }

  // if (rainValue < 800) {
  //   message += "It's raining. ";

  // } else {
  //   message += "No rain. ";
  // }

  if (pirValue == HIGH) {
    message += "Motion Detected!";
  } else {
    message += "No Motion. ";
  }


  server.send(200, "text/plain", message);
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

  // sr.set(GUEST_LED, true);
}

void turnOffGuestLed() {
  digitalWrite(TX_GUEST_LED, LOW); // Pornește complet LED-ul

  // sr.set(GUEST_LED, false);
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
  // sr.set(BEDROOM_LED, true);
  digitalWrite(RX_BEDROOM_LED, HIGH); 

}

void turnOffBedroomLed() {
  // sr.set(BEDROOM_LED, false);
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
void setGuestDoorPosition(int position) {
  guestDoorPosition = position;

}

void openGuestDoor() {
    setServoPosition(GUEST_DOOR, 170);

}

void closeGuestDoor() {
    setServoPosition(GUEST_DOOR, 20);

}


//FrontDoor
void setFrontDoorPosition(int position) {
  frontDoorPosition = position;
}

void openFrontDoor() {
    // setServoPosition(FRONT_DOOR, 140); 
        setServoPosition(FRONT_DOOR, 170); 

}

void closeFrontDoor() {
  //  setServoPosition(FRONT_DOOR, 21); 
      setServoPosition(FRONT_DOOR, 55); 

}

//BedRoomDoor
void setBedRoomDoorPosition(int position) {
  bedRoomDoorPosition = position;
}

void openBedRoomDoor() {
  setServoPosition(BED_ROOM_DOOR, 20);

}

void closeBedRoomDoor() {
  // setBedRoomDoorPosition(145); 
              setServoPosition(BED_ROOM_DOOR, 150);

}


//Windows
void setGuestWindowPosition(int position) {
  guestWindowPosition = position;
}

void openGuestWindow() {
  // setGuestWindowPosition(25);
            setServoPosition(GUEST_WINDOW, 25);

}

void closeGuestWindow() {
  // setGuestWindowPosition(110);
          setServoPosition(GUEST_WINDOW, 104);

}

void setBedRoomWindowPosition(int position) {
  bedRoomWindowPosition = position;
}

void openBedRoomWindow() {
  // setBedRoomWindowPosition(130);
          setServoPosition(BED_ROOM_WINDOW, 135);

}

void closeBedRoomWindow() {
  // setBedRoomWindowPosition(25);
        setServoPosition(BED_ROOM_WINDOW, 28);

}


void setLivingWindowPosition(int position) {
  livingWindowPosition = position;
}

void openLivingWindow() {
  // setLivingWindowPosition(115);
      setServoPosition(LIVING_WINDOW, 110);

}

void closeLivingWindow() {
  // setLivingWindowPosition(10);
      setServoPosition(LIVING_WINDOW, 12); 

}


const unsigned long pwmPeriod = 20000; // 20ms în microsecunde
const int pulseMin = 400;  
const int pulseMax = 2600;


void setServoPosition(int pin, int position) {
    int pulseWidth = map(position, 0, 180, pulseMin, pulseMax);
    ///Daca decomentez cele 2 serial.print nu mai se reseteaza placuta (watchdog)
    // Serial.print("Setare servo poziție: ");
    // Serial.println(position);

    // Generăm semnal PWM timp de 1 secundă (50 cicluri la 20 ms fiecare)
    for (int i = 0; i < 50; i++) {
        sr.set(pin, HIGH);  // Pin HIGH pentru durata impulsului
        delayMicroseconds(pulseWidth);
        sr.set(pin, LOW);   // Pin LOW pentru restul perioadei
        delayMicroseconds(pwmPeriod - pulseWidth);
        yield(); // Previne resetarea watchdog-ului

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
    if (isStepperMotorRunning) return; // Nu permite alte comenzi dacă motorul este activ
    isStepperMotorRunning = true;

    long steps = (220L * 2048L) / 360L; // 200 de grade -> pași
    stepper.setStepsToMake(steps, true);

    // Când motorul termină, setează isMotorRunning pe false
    while (stepper.isRunning()) {
        stepper.update();
        setNewStepperState();
        yield(); // Permite altor procese să ruleze
    }

    isStepperMotorRunning = false; // Motorul a terminat execuția
    deactivateStepper();
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
        yield(); // Permite altor procese să ruleze
    }

    isStepperMotorRunning = false; // Motorul a terminat execuția
    deactivateStepper();
}

void deactivateStepper() {
  sr.set(IN1, LOW);
  sr.set(IN2, LOW);
  sr.set(IN3, LOW);
  sr.set(IN4, LOW);
}


unsigned long lastMotionTime = 0;  // Variabilă pentru a stoca timpul ultimei mișcări detectate
const unsigned long lightOnDuration = 5000;  // Durata în milisecunde (5 secunde)

void checkLightAndMotion() {
    static bool lastMotionDetected = false;
    static bool lastIsNight = false;

    bool currentMotionDetected = (digitalRead(PIR_PIN) == HIGH);
    bool currentIsNight = (digitalRead(LDR_PIN) == HIGH);

    if (currentMotionDetected != lastMotionDetected) {
        String motionStatus = currentMotionDetected ? "Motion Detected!" : "No Motion";
        Firebase.setString(firebaseData, "/sensorData/motion", motionStatus);
        lastMotionDetected = currentMotionDetected;
        // Serial.println("Actualizat starea senzorului de mișcare în Firebase.");
    }

    if (currentIsNight != lastIsNight) {
        String lightStatus = currentIsNight ? "Night" : "Day";
        Firebase.setString(firebaseData, "/sensorData/isDay", lightStatus);
        lastIsNight = currentIsNight;
        // Serial.println("Actualizat starea de lumină (zi/noapte) în Firebase.");
    }

    if (currentMotionDetected && currentIsNight) {
        turnOnHallLed();
        lastMotionTime = millis();  
    }

  // Verificăm dacă au trecut 10 secunde de la ultima mișcare detectată
  if (millis() - lastMotionTime > lightOnDuration) {
    turnOffHallLed();
  }
}

void  displayTemperatureHumidity() {
    static float lastTemperature = -999; // Valoare imposibilă pentru inițializare
    static float lastHumidity = -999; // Valoare imposibilă pentru inițializare

    float currentTemperature = dht.readTemperature();
    float currentHumidity = dht.readHumidity();

    // Verificăm dacă citirea senzorului este validă
    if (isnan(currentTemperature) || isnan(currentHumidity)) {
        Serial.println("Eroare la citirea senzorului DHT!");
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
String childPath[17] = {
  "/led/guest/ledDim",
  "/led/bedRoom/ledDim",
  "/led/guest/led",
  "/led/bedRoom/led",
  "/led/garageLed",
  "/led/doorLed",
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
  "/statusESP8266/reset",
};




String lastGarageDoorState = "close"; // Starea inițială presupusă

// Store brightness (0-100 range)
int brightnessTX = 0;
int brightnessRX = 0;

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

            // Convert the value from string to integer
            int brightnessPercentage = value.toInt();

            // Validate brightness percentage (must be between 0 and 100)
            if (brightnessPercentage < 0 || brightnessPercentage > 100) {
                // Serial.printf("Invalid brightness value for %s: %d\n", path.c_str(), brightnessPercentage);
                continue;
            }

            // Handle each path
            if (path == "/led/guest/ledDim") {
                int pwmValue = map(brightnessPercentage, 0, 100, 0, 255); // Map to 0-255
                analogWrite(TX_GUEST_LED, pwmValue); // Set TX LED (garage)
                brightnessTX = brightnessPercentage;
                // Serial.printf("Garage LED brightness set to %d%% (PWM: %d)\n", brightnessPercentage, pwmValue);

            } else if (path == "/led/bedRoom/ledDim") {
                int pwmValue = map(brightnessPercentage, 0, 100, 0, 255); // Map to 0-255
                analogWrite(RX_BEDROOM_LED, pwmValue); // Set RX LED (guest)
                brightnessRX = brightnessPercentage;
                // Serial.printf("Guest LED brightness set to %d%% (PWM: %d)\n", brightnessPercentage, pwmValue);
       
            } else
            if (path == "/led/garageLed") {
                value == "on" ? turnOnGarageLed() : turnOffGarageLed();
            } else if (path == "/led/guest/led") {
                value == "on" ? turnOnGuestLed() : turnOffGuestLed();
            } else if (path == "/led/doorLed") {
                value == "on" ? turnOnDoorLed() : turnOffDoorLed();
            } else if (path == "/led/bedRoom/led") {
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
                  //  value == "open" ? handleOpen() : handleClose();

            } else if (path == "/door/bedRoomDoor") {
                value == "open" ? openBedRoomDoor() : closeBedRoomDoor();
            } else if (path == "/door/garage") {
                // Gestionăm comanda doar dacă valoarea s-a schimbat
                if (value != lastGarageDoorState) {
                    lastGarageDoorState = value;

                    if (value == "open") {
                        // Serial.println("Comanda primită: open - motorul va merge backward.");
                        handleBackward();
                    } else if (value == "close") {
                        // Serial.println("Comanda primită: close - motorul va merge forward.");
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

            // Control motor
            else if (path == "/motor/fan") {
                value == "start" ? startFan() : stopFan();
            }
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


void showWiFiSignalStrength() {
    int rssi = WiFi.RSSI(); 
    Firebase.setString(firebaseData, "/statusESP8266/wifiSignal", 
        "RSSI: " + String(rssi) + " dBm");
}


void showFreeHeap() {
    static String lastRamStatus = "";

    int freeRam = ESP.getFreeHeap();
    int totalRam = 50000; 
    float freePercentage = ((float)freeRam / totalRam) * 100;

    String currentRamStatus = String(freeRam) + " bytes (" + String(freePercentage, 2) + "%)";

    if (currentRamStatus != lastRamStatus) {
        Firebase.setString(firebaseData, "/statusESP8266/ram", currentRamStatus);
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


    // Dimensiunea totală Flash
    Firebase.setString(firebaseData, "/statusESP8266/flash/totalSize", String(ESP.getFlashChipSize()) + " bytes");

    // Dimensiunea libera Flash
    Firebase.setString(firebaseData, "/statusESP8266/flash/freeSize", String(ESP.getFreeSketchSpace()) + " bytes");

    // Versiunea SDK
    Firebase.setString(firebaseData, "/statusESP8266/sdkVersion", ESP.getSdkVersion());

}

void handleReset() {
    ESP.restart();
}

void displayHallSensorState() {
  int hallState = digitalRead(HALL_SENSOR_PIN); 
  if (hallState == LOW) {
    Firebase.setString(firebaseData, "/doorStatus", "closed");

  } else {
    Firebase.setString(firebaseData, "/doorStatus", "opened");

  }
}



void activateBuzzer() {
  sr.set(BUZZER_PIN, HIGH); 
}

void deactivateBuzzer() {
  sr.set(BUZZER_PIN, LOW); 
}



// Setup function
void setup() {
  Serial.begin(115200);
  Serial.println("Motivul resetării: " + ESP.getResetReason());

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


  // stream.setBSSLBufferSize(512, 128);
  stream.setBSSLBufferSize(1024, 256); // Crește bufferul dacă datele sunt mari

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
    reportResetReason();

  // Setarea MultiPath Stream
if (!Firebase.beginMultiPathStream(stream, parentPath)) {
    Serial.printf("Stream begin error, %s\n\n", stream.errorReason().c_str());
  }

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);

  Serial.println("Connected to Firebase");

  reportStaticInfo();


    // Configure pins for DIM led
  pinMode(TX_GUEST_LED, OUTPUT);
  pinMode(RX_BEDROOM_LED, OUTPUT);

  pinMode(LDR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);


  pinMode(HALL_SENSOR_PIN,INPUT_PULLUP); 
   
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);

  // Define routes for the web server
  server.on("/", handleRoot);
  server.on("/temperature_humidity", handleTemperatureHumidity);
  server.on("/sensors_status", handleSensorsStatus);

  //Buzzer
  server.on("/onBuzzer", activateBuzzer);
  server.on("/OffBuzzer", deactivateBuzzer);
  // server.on("/onBuzzer1", activateBuzzer1);
  // server.on("/OffBuzzer1", deactivateBuzzer1);

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
unsigned long lastHallSensorReadTime = 0;
const unsigned long hallSensorInterval = 2000; // 2 secunde

unsigned long lastSensorReadTime = 0;
const unsigned long sensorReadInterval = 5000; // 5 secunde



void loop() {
  server.handleClient();

  stepper.update();
  if (stepper.isUpdated) {
    setNewStepperState();
    stepper.clearUpdated();
  } 

  unsigned long currentTime = millis();
  // Citirea senzorilor la intervalul definit
   if (currentTime - lastHallSensorReadTime >= hallSensorInterval) {
    lastHallSensorReadTime = currentTime;
    displayHallSensorState();
  }

  if (currentTime - lastSensorReadTime >= sensorReadInterval) {
    lastSensorReadTime = currentTime;
    readAllSensors();
  }
}

void readAllSensors() {
    checkLightAndMotion();
    displayTemperatureHumidity();
    checkFire();
    checkGas();
    checkRain();
    showFreeHeap();  
    showWiFiSignalStrength();

}

void reportResetReason() {
    String resetReason = ESP.getResetReason();

    // // Afișăm motivul resetării în Serial Monitor
    // Serial.print("Motivul resetării: ");
    // Serial.println(resetReason);

    // Trimitem motivul resetării pe Firebase
 Firebase.setString(firebaseData, "/statusESP8266/resetReason", resetReason);
}

