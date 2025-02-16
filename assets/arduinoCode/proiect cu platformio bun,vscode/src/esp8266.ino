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

#include <ArduinoOTA.h>



// WiFi credentials
// const char* ssid = "Seco";
// const char* password = "secosanpq1";


// WiFi credentials
const char* ssid = "xSecoI";
const char* password = "seco1212";

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

// #define TX_GUEST_LED D1  // GPIO1
// #define RX_BEDROOM_LED D2  // GPIO3

// #define DHT_PIN 1
// #define PIR_PIN 3  

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

  if (fireState == LOW) {
    message += "Fire detected! ";

  } else {
    message += "No fire. ";
  }

  message += "Gas Value: " + String(gasState) + "\n";
  message += "Rain Value: " + String(rainValue) + "\n";
 
  server.send(200, "text/plain", message);
}

void handleMotionStatus() {
  String message = "Status: ";
  
  int pirValue = digitalRead(PIR_PIN);
  int hallState = digitalRead(HALL_SENSOR_PIN); 

  message += "Door Status: " + String(hallState) + "\n";

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
    setServoPosition(GUEST_DOOR, 158);

}

void closeGuestDoor() {
    setServoPosition(GUEST_DOOR, 24);

}


//FrontDoor
void setFrontDoorPosition(int position) {
  frontDoorPosition = position;
}

void openFrontDoor() {
    setServoPosition(FRONT_DOOR, 135); 
        // setServoPosition(FRONT_DOOR, 170); 

}

void closeFrontDoor() {
   setServoPosition(FRONT_DOOR, 31); 
      // setServoPosition(FRONT_DOOR, 55); 

}

//BedRoomDoor
void setBedRoomDoorPosition(int position) {
  bedRoomDoorPosition = position;
}

void openBedRoomDoor() {
  setServoPosition(BED_ROOM_DOOR, 28);

}

void closeBedRoomDoor() {
  // setBedRoomDoorPosition(145); 
              setServoPosition(BED_ROOM_DOOR, 165);

}


//Windows
void setGuestWindowPosition(int position) {
  guestWindowPosition = position;
}

void openGuestWindow() {
   setServoPosition(GUEST_WINDOW, 5);
}

void closeGuestWindow() {
    setServoPosition(GUEST_WINDOW, 89);
}

void setBedRoomWindowPosition(int position) {
  bedRoomWindowPosition = position;
}

void openBedRoomWindow() {
    setServoPosition(BED_ROOM_WINDOW, 130);
}

void closeBedRoomWindow() {
    setServoPosition(BED_ROOM_WINDOW, 48);
}


void setLivingWindowPosition(int position) {
  livingWindowPosition = position;
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

    long steps = (205L * 2048L) / 360L; // 200 de grade -> pași
    stepper.setStepsToMake(steps, true);

    // Când motorul termină, setează isMotorRunning pe false
    while (stepper.isRunning()) {
        stepper.update();
        setNewStepperState();
        yield(); // Permite altor procese să ruleze
    }
    
    // deactivateStepper();
    isStepperMotorRunning = false; // Motorul a terminat execuția

     if (!stepper.isRunning()) {
      deactivateStepper();
    }
}

void handleBackward() {
    if (isStepperMotorRunning) return; // Nu permite alte comenzi dacă motorul este activ
    isStepperMotorRunning = true;

    long steps = (190L * 2048L) / 360L; // 190 de grade -> pași
    stepper.setStepsToMake(steps, false);

    // Când motorul termină, setează isMotorRunning pe false
    while (stepper.isRunning()) {
        stepper.update();
        setNewStepperState();
        yield(); // Permite altor procese să ruleze
    }

    // deactivateStepper();
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



// void checkLightAndMotion() {
//     static bool lastMotionDetected = false;
//     static bool lastIsNight = false;
//     static bool lastManualControlHallLed = false;

//     bool currentMotionDetected = (digitalRead(PIR_PIN) == HIGH);
//     bool currentIsNight = (digitalRead(LDR_PIN) == HIGH);

//     if (currentMotionDetected != lastMotionDetected) {
//         String motionStatus = currentMotionDetected ? "Motion Detected!" : "No Motion";
//         Firebase.setString(firebaseData, "/sensorData/motion", motionStatus);
//         lastMotionDetected = currentMotionDetected;
//         // Serial.println("Actualizat starea senzorului de mișcare în Firebase.");
//     }

//     if (currentIsNight != lastIsNight) {

//         String lightStatus = "";
//       if (currentIsNight) {
//             manualControlHallLed = false;
//             Firebase.setString(firebaseData, "/led/hallLedStatus", "auto");
//             lightStatus = "Night";
//         } else {
          
//           Firebase.setString(firebaseData, "/led/hallLedStatus", "manual");
//           lightStatus = "Day";
//           manualControlHallLed = true;
//         }

//         Firebase.setString(firebaseData, "/sensorData/isDay", lightStatus);
//         lastIsNight = currentIsNight;
//         // Serial.println("Actualizat starea de lumină (zi/noapte) în Firebase.");
//     }

//     if (currentMotionDetected && currentIsNight && !manualControlHallLed) {
//         turnOnHallLed();
//         lastMotionTime = millis();  
//     }

//     // Dacă e noapte, nu e mișcare și nu e modul manual, stingem LED-ul după o perioadă
//         if (!currentMotionDetected && currentIsNight && !manualControlHallLed && millis() - lastMotionTime > lightOnDuration) {
//             turnOffHallLed();
//         }
// }



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
    Firebase.setString(firebaseData, "/security/status", "armed");

    // Verificăm dacă există mișcare sau ușa este deschisă
    bool currentMotionDetected = (digitalRead(PIR_PIN) == HIGH);
    bool isHallDoorOpen = (digitalRead(HALL_SENSOR_PIN) == HIGH); // Ușa este deschisă dacă starea este HIGH

    if (currentMotionDetected || isHallDoorOpen) {
        if (!isAlarmTriggered) {
            activateBuzzer(); // Activăm alarma doar dacă nu a fost deja activată
            String alertMessage = "Alarm!!";
            if (currentMotionDetected) {
                alertMessage += " Mișcare detectată!";
            }
            if (isHallDoorOpen) {
                alertMessage += " Ușa este deschisă!";
            }
            Firebase.setString(firebaseData, "/security/alert", alertMessage);
            isAlarmTriggered = true; // Setăm st area alarmei ca fiind declanșată
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




FirebaseData fbdo;
FirebaseData stream;

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




String lastGarageDoorState = "open"; // Starea inițială presupusă



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

            // // Validate brightness percentage (must be between 0 and 100)
            // if (brightnessPercentage < 0 || brightnessPercentage > 100) {
            //     // Serial.printf("Invalid brightness value for %s: %d\n", path.c_str(), brightnessPercentage);
            //     continue;
            // }

            // Handle each path
            if (path == "/led/guest/ledDim") {
                // int pwmValue = map(brightnessPercentage, 0, 100, 0, 255); // Map to 0-255
                // analogWrite(TX_GUEST_LED, pwmValue); // Set TX LED (garage)

                handleLedWithPWM(TX_GUEST_LED, brightnessPercentage); 
                brightnessTX = brightnessPercentage;
                // Serial.printf("Garage LED brightness set to %d%% (PWM: %d)\n", brightnessPercentage, pwmValue);

            } else if (path == "/led/bedRoom/ledDim") {
                // int pwmValue = map(brightnessPercentage, 0, 100, 0, 255); // Map to 0-255
                // analogWrite(RX_BEDROOM_LED, pwmValue); // Set RX LED (guest)
                
                handleLedWithPWM(RX_BEDROOM_LED, brightnessPercentage); // Gestionare pentru LED-ul de la bedroom
                brightnessRX = brightnessPercentage;
                // Serial.printf("Guest LED brightness set to %d%% (PWM: %d)\n", brightnessPercentage, pwmValue);
       
            } else
            if (path == "/led/garageLed") {
                value == "on" ? turnOnGarageLed() : turnOffGarageLed();
            } 
            else if (path == "/led/doorLed") {
                value == "on" ? turnOnDoorLed() : turnOffDoorLed();
            } 
          
            else if (path == "/led/hallLed") {
                if (value == "on") {
                    turnOnHallLed();
                    manualControlHallLed = true; 
                    Firebase.setString(firebaseData, "/led/hallLedStatus", "manual");

                } else if (value == "off") {
                    turnOffHallLed();
                    manualControlHallLed = true; 
                    Firebase.setString(firebaseData, "/led/hallLedStatus", "manual");

                 } 
               
                }
            
               else if (path == "/led/hallLedStatus") {
                if (value == "auto") {
                    manualControlHallLed = false; // Permitem controlul automat
                   }
                else if (value == "manual"){
                      manualControlHallLed = true; 

                }
               }

            
            else if (path == "/led/livingLed") {
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
            // else if (path == "/motor/fan") {
            //     value == "start" ? startFan() : stopFan();
            // }

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
                      // armHouse();
                          isHouseArmed = true; 
                              Firebase.setString(firebaseData, "/security/alert", "Clear");



                } else if (value == "disarm") {
                    disarmHouse(); 
                }
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

void displayHallSensorState() {
  static int lastHallState = -1; // Variabilă pentru stocarea stării anterioare

  int hallState = digitalRead(HALL_SENSOR_PIN); 
  if (hallState != lastHallState) { // Actualizăm Firebase doar dacă starea s-a schimbat
    String doorStatus = (hallState == LOW) ? "Closed" : "Opened";
    Firebase.setString(firebaseData, "/sensorData/doorStatus", doorStatus);
    lastHallState = hallState; // Actualizăm starea anterioară
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

  WiFi.mode(WIFI_STA);

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


  // stream.setBSSLBufferSize(512, 128); nu 
  stream.setBSSLBufferSize(1024, 256); // Crește bufferul dacă datele sunt mari
  // stream.setBSSLBufferSize(2048, 256);//e ok 
// 4096 sau chiar 8192 dacă datele sunt semnificativ mai mari la prima valoare la citire

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
  reportResetReason();

  // Setarea MultiPath Stream
if (!Firebase.beginMultiPathStream(stream, parentPath)) {
    Serial.printf("Stream begin error, %s\n\n", stream.errorReason().c_str());
  }

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);

  Serial.println("Connected to Firebase");

  Firebase.setString(firebaseData, "/statusESP8266/reset", "no reset");

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
  server.on("/motion_status", handleMotionStatus);

  

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

  // Configurare OTA
  // Nume device (opțional)
    ArduinoOTA.setHostname("esp8266_Raul");
  // Parolă OTA (opțional):
    ArduinoOTA.setPassword("xseco");
    ArduinoOTA.begin();

}


// Main loop
// Timere pentru senzori 
unsigned long lastHallSensorReadTime = 0;
const unsigned long hallSensorInterval = 2000; // 2 secunde

unsigned long lastSensorReadTime = 0;
const unsigned long sensorReadInterval = 5000; // 5 secunde



void loop() {
  server.handleClient();

  unsigned long currentTime = millis();
  // Citirea senzorilor la intervalul definit
   if (currentTime - lastHallSensorReadTime >= hallSensorInterval) {
    lastHallSensorReadTime = currentTime;
    displayHallSensorState();
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