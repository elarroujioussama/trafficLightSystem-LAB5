#include <Wire.h>

const int BUTTON_PIN = 2;
const int PED_GREEN = 9;
const int PED_RED = 10;
volatile bool pedestrianRequest = false;
volatile unsigned long lastInterruptTime = 0;

void setup() {
  pinMode(PED_GREEN, OUTPUT);
  pinMode(PED_RED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button with an internal pull-up

  Wire.begin(8); // Act as a slave with address 8
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  // Start with pedestrian light on RED
  digitalWrite(PED_RED, HIGH);
  digitalWrite(PED_GREEN, LOW);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, FALLING);
}

void loop() {
  delay(10);
}

void buttonPressed() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > 200) { // Debounce time
    pedestrianRequest = true;
    lastInterruptTime = currentTime;
  }
}

void requestEvent() {
  if (pedestrianRequest) {
    Wire.write(1); // There's a pending request
    pedestrianRequest = false; // Reset the flag after informing the master
  } else {
    Wire.write(0); // No request
  }
}

void receiveEvent(int bytes) {
  char command = Wire.read();
  if (command == 'R') {
    digitalWrite(PED_RED, LOW);
    digitalWrite(PED_GREEN, HIGH);
  } else if (command == 'G') {
    digitalWrite(PED_RED, HIGH);
    digitalWrite(PED_GREEN, LOW);
  }
}
