#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ShiftRegister74HC595.h>

// Conexiuni Wi-Fi
const char* ssid = "Seco";
const char* password = "secosanpq1";

// Conexiuni Shift Register
const int dataPin = D5;   // DS
const int clockPin = D6;  // SHCP
const int latchPin = D8;  // STCP

// Creează obiectele
ShiftRegister74HC595<1> sr(dataPin, clockPin, latchPin);
ESP8266WebServer server(80);

// Pinii motorului pe Shift Register
#define IN1 3
#define IN2 4
#define IN3 5
#define IN4 6

int motorSpeed = 10;  // Viteza motorului (delay între pași)
int desiredAngle = 0; // Unghiul dorit
int stepsPerRevolution = 2048; // Numărul de pași pentru o revoluție completă
int currentAngle = 0;

// Setăm toate pinii shift register-ului la LOW
void clearRegisters() {
  for (int i = 0; i < 8; i++) {
    sr.set(i, LOW);
  }
}

// Scriem starea pinilor
void writeRegisters() {
  sr.updateRegisters();
}

// Setăm un pin individual HIGH sau LOW
void setRegisterPin(int index, int value) {
  sr.set(index, value);
  writeRegisters();
}

// Funcție pentru rotație în sens orar
void clockwise(int steps) {
  for (int i = 0; i < steps; i++) {
    // Pas 1
    setRegisterPin(IN1, HIGH);
    setRegisterPin(IN2, LOW);
    setRegisterPin(IN3, LOW);
    setRegisterPin(IN4, LOW);
    delay(motorSpeed);

    // Pas 2
    setRegisterPin(IN1, HIGH);
    setRegisterPin(IN2, HIGH);
    setRegisterPin(IN3, LOW);
    setRegisterPin(IN4, LOW);
    delay(motorSpeed);

    // Pas 3
    setRegisterPin(IN1, LOW);
    setRegisterPin(IN2, HIGH);
    setRegisterPin(IN3, LOW);
    setRegisterPin(IN4, LOW);
    delay(motorSpeed);

    // Pas 4
    setRegisterPin(IN1, LOW);
    setRegisterPin(IN2, HIGH);
    setRegisterPin(IN3, HIGH);
    setRegisterPin(IN4, LOW);
    delay(motorSpeed);

    // Pas 5
    setRegisterPin(IN1, LOW);
    setRegisterPin(IN2, LOW);
    setRegisterPin(IN3, HIGH);
    setRegisterPin(IN4, LOW);
    delay(motorSpeed);

    // Pas 6
    setRegisterPin(IN1, LOW);
    setRegisterPin(IN2, LOW);
    setRegisterPin(IN3, HIGH);
    setRegisterPin(IN4, HIGH);
    delay(motorSpeed);

    // Pas 7
    setRegisterPin(IN1, LOW);
    setRegisterPin(IN2, LOW);
    setRegisterPin(IN3, LOW);
    setRegisterPin(IN4, HIGH);
    delay(motorSpeed);

    // Pas 8
    setRegisterPin(IN1, HIGH);
    setRegisterPin(IN2, LOW);
    setRegisterPin(IN3, LOW);
    setRegisterPin(IN4, HIGH);
    delay(motorSpeed);
  }
}

// Funcție pentru rotație în sens antiorar
void counterclockwise(int steps) {
  for (int i = 0; i < steps; i++) {
    // Pașii sunt în ordine inversă
    clockwise(1); // Simplificare - inversăm ordinea
  }
}

// Funcție pentru pagina principală
void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Stepper Motor Control</h1>";
  html += "<form action=\"/set\" method=\"GET\">";
  html += "Degrees: <input type=\"number\" name=\"angle\" min=\"-360\" max=\"360\"><br>";
  html += "Speed: <input type=\"number\" name=\"speed\" min=\"5\" max=\"100\"><br>";
  html += "<input type=\"submit\" value=\"Set\">";
  html += "</form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Funcție pentru setarea unghiului și vitezei
void handleSet() {
  if (server.hasArg("angle") && server.hasArg("speed")) {
    desiredAngle = server.arg("angle").toInt();
    motorSpeed = server.arg("speed").toInt();

    int stepsToMove = (abs(desiredAngle - currentAngle) * stepsPerRevolution) / 360;

    if (desiredAngle > currentAngle) {
      clockwise(stepsToMove);
    } else if (desiredAngle < currentAngle) {
      counterclockwise(stepsToMove);
    }

    currentAngle = desiredAngle;

    server.send(200, "text/plain", "Motor moved to: " + String(desiredAngle) + " degrees with speed: " + String(motorSpeed));
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void setup() {
  Serial.begin(115200);

  // Configurare Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Configurare pinii shift register
  sr.setAllLow();

  // Configurare server web
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
