#define HALL_SENSOR_PIN D7

void displayHallSensorState() {
  int hallState = digitalRead(HALL_SENSOR_PIN); 
      Serial.println(hallState);

  // if (hallState == LOW) {
  //   Serial.println("Magnet detected (Hall sensor state: LOW)");
  //   // Firebase.setString(firebaseData, "/doorStatus", "closed");

  // } else {
  //   Serial.println("No magnet detected (Hall sensor state: HIGH)");
  //   // Firebase.setString(firebaseData, "/doorStatus", "opened");

  // }
}


void setup() {
  Serial.begin(115200);


  pinMode(HALL_SENSOR_PIN,INPUT_PULLUP); 


}

void  loop(){
   displayHallSensorState();
   delay(500);
}