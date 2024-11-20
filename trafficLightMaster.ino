#include <Wire.h>

const int PIN_GREEN = 11;
const int PIN_YELLOW = 12;
const int PIN_RED = 13;
long t20 = 10000;
unsigned long previousMillis = 0;
bool pedestrianRequest = false;

enum State {
  GREEN,
  YELLOW,
  RED
} currentState = GREEN;

void setup() {
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_RED, OUTPUT);

  Wire.begin(); // Start I2C as master
  green();

  // Inform slave to start with pedestrian red light
  Wire.beginTransmission(8);
  Wire.write('G');
  Wire.endTransmission();
}

void loop() {
  requestPedestrian();
  delay(10); // Delay included to mitigate potential I2C communication issues
  unsigned long currentMillis = millis();

  switch (currentState) {
    case GREEN:
      if ((currentMillis - previousMillis >= t20) || pedestrianRequest) {
        yellow();
        currentState = YELLOW;
        previousMillis = currentMillis;
      }
      break;
    case YELLOW:
      if (currentMillis - previousMillis >= 3000) {
        red();
        currentState = RED;
        previousMillis = currentMillis;
        Wire.beginTransmission(8);
        Wire.write('R');
        Wire.endTransmission();
      }
      break;
    case RED:
      if (currentMillis - previousMillis >= t20) {
        green();
        currentState = GREEN;
        previousMillis = currentMillis;
        Wire.beginTransmission(8);
        Wire.write('G');
        Wire.endTransmission();
        pedestrianRequest = false;
      }
      break;
  }
}

void green() {
  digitalWrite(PIN_GREEN, HIGH);
  digitalWrite(PIN_YELLOW, LOW);
  digitalWrite(PIN_RED, LOW);
}

void yellow() {
  digitalWrite(PIN_GREEN, LOW);
  digitalWrite(PIN_YELLOW, HIGH);
  digitalWrite(PIN_RED, LOW);
}

void red() {
  digitalWrite(PIN_GREEN, LOW);
  digitalWrite(PIN_YELLOW, LOW);
  digitalWrite(PIN_RED, HIGH);
}

void requestPedestrian() {
  Wire.requestFrom(8, 1); // Request 1 byte from slave with address 8
  if (Wire.available()) {
    int request = Wire.read();
    pedestrianRequest = (request == 1);
  }
}
