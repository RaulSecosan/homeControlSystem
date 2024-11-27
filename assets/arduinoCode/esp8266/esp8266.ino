#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ShiftRegister74HC595.h>
#include <DHT.h>
#include <Firebase.h>

// WiFi credentials
const char* ssid = "Seco";
const char* password = "secosanpq1";


#define FIREBASE_HOST "licentalivedb-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "K9re6wdo8C5r860LD0TnPoSAo2I96n1NgMuFP3wJ"

FirebaseData fbdo;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;


// Pins for 74HC595
const int dataPin = D5;
const int clockPin = D6;
const int latchPin = D8;

// Create ShiftRegister74HC595 object with 3 shift registers
ShiftRegister74HC595<3> sr(dataPin, clockPin, latchPin);

// PWM dimming on TX and RX
#define TX_LED 1  // GPIO1
#define RX_LED 3  // GPIO3

// Store brightness (0-100 range)
int brightnessTX = 0;
int brightnessRX = 0;

// Web server
ESP8266WebServer server(80);




// Path pentru Firebase
String parentPath = "/";
String childPath[3] = {
    "led/garageLedDim",
    "led/guestDim",
    "/led/bedroomLedDim",
};
void streamCallback(MultiPathStreamData stream) {
    size_t numChild = sizeof(childPath) / sizeof(childPath[0]);

    for (size_t i = 0; i < numChild; i++) {
        if (stream.get(childPath[i])) {
            String path = stream.dataPath;
            String value = stream.value.c_str();

            // Convert the value from string to integer
            int brightnessPercentage = value.toInt();

            // Validate brightness percentage (must be between 0 and 100)
            if (brightnessPercentage < 0 || brightnessPercentage > 100) {
                Serial.printf("Invalid brightness value for %s: %d\n", path.c_str(), brightnessPercentage);
                continue;
            }

            // Handle each path
            if (path == "/led/garageLedDim") {
                int pwmValue = map(brightnessPercentage, 0, 100, 0, 255); // Map to 0-255
                analogWrite(TX_GUEST_LED, pwmValue); // Set TX LED (garage)
                brightnessTX = brightnessPercentage;
                Serial.printf("Garage LED brightness set to %d%% (PWM: %d)\n", brightnessPercentage, pwmValue);

            } else if (path == "/led/guestDim") {
                int pwmValue = map(brightnessPercentage, 0, 100, 0, 255); // Map to 0-255
                analogWrite(RX_BEDROOM_LED, pwmValue); // Set RX LED (guest)
                brightnessRX = brightnessPercentage;
                Serial.printf("Guest LED brightness set to %d%% (PWM: %d)\n", brightnessPercentage, pwmValue);

            } else if (path == "/led/bedroomLedDim") {
                // Example: Update other LEDs connected to the shift register
                int pwmValue = map(brightnessPercentage, 0, 100, 0, 255); // Map to 0-255
                sr.set(15, brightnessPercentage > 50); // Example threshold
                sr.updateRegisters();
                Serial.printf("Bedroom LED brightness managed by shift register (Brightness: %d%%)\n", brightnessPercentage);

            } else {
                // Unknown path
                Serial.printf("Unknown command on path: %s, value: %s\n", path.c_str(), value.c_str());
            }
        }
    }
}


// Firebase stream timeout callback
void streamTimeoutCallback(bool timeout) {
    if (timeout) {
        Serial.println("Stream timed out, resuming...");
    }
}

void setup() {
  Serial.begin(115200);

  // Configure pins for PWM
  pinMode(TX_GUEST_LED, OUTPUT);
  pinMode(RX_BEDROOM_LED, OUTPUT);

  // WiFi setup
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());
firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);

    if (!Firebase.beginMultiPathStream(fbdo, parentPath)) {
        Serial.printf("Stream begin error: %s\n", fbdo.errorReason().c_str());
    } else {
        Firebase.setMultiPathStreamCallback(fbdo, streamCallback, streamTimeoutCallback);
    }

  // Web server routes
  server.on("/", []() {
    String html = "<h1>Control LEDs</h1>";
    html += "<form action='/set' method='GET'>";
    html += "TX LED Brightness (0-100): <input type='number' name='tx' min='0' max='100' value='" + String(brightnessTX) + "'><br>";
    html += "RX LED Brightness (0-100): <input type='number' name='rx' min='0' max='100' value='" + String(brightnessRX) + "'><br>";
    html += "<input type='submit' value='Set Brightness'>";
    html += "</form>";
    server.send(200, "text/html", html);
  });

  server.on("/set", []() {
    if (server.hasArg("tx")) {
      int inputTX = server.arg("tx").toInt();
      if (inputTX >= 0 && inputTX <= 100) {
        brightnessTX = inputTX;
        int pwmValue = map(brightnessTX, 0, 100, 0, 255);  // Map 0-100 to 0-255
        analogWrite(TX_GUEST_LED, pwmValue);
        Serial.printf("TX LED brightness set to %d%% (PWM: %d)\n", brightnessTX, pwmValue);
      } else {
        Serial.println("Invalid TX value. Must be between 0-100.");
      }
    }
    if (server.hasArg("rx")) {
      int inputRX = server.arg("rx").toInt();
      if (inputRX >= 0 && inputRX <= 100) {
        brightnessRX = inputRX;
        int pwmValue = map(brightnessRX, 0, 100, 0, 255);  // Map 0-100 to 0-255
        analogWrite(RX_BEDROOM_LED, pwmValue);
        Serial.printf("RX LED brightness set to %d%% (PWM: %d)\n", brightnessRX, pwmValue);
      } else {
        Serial.println("Invalid RX value. Must be between 0-100.");
      }
    }
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
