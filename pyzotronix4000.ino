#include <Arduino.h>
#define IR_INPUT_PIN 2
#define VOL_UP_PIN 3
#define VOL_DOWN_PIN 4
#define POWER_PIN 5
#define TOGGLE_PIN 6

#include "TinyIRReceiver.hpp"

bool powerOn = false;
bool volUp = false;
bool volDown = false;

void setup() {
  // Serial.begin(115200);
  pinMode(VOL_UP_PIN, OUTPUT);
  pinMode(VOL_DOWN_PIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(TOGGLE_PIN, INPUT_PULLUP);
  digitalWrite(VOL_DOWN_PIN, LOW);
  digitalWrite(VOL_UP_PIN, LOW);
  digitalWrite(POWER_PIN, LOW);
  initPCIInterruptForTinyReceiver();
}

void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat) {
  switch (aCommand) {
    case 0:
      if (!isRepeat) {
         togglePower();
      }
      break;
    case 4:
      volUp = true; break;
    case 8:
      volDown = true; break;
  }
}

void loop() {
  if (digitalRead(TOGGLE_PIN) == LOW) {
    delay(200);
    togglePower();
  }
  if (volUp) {
    turnVolume(VOL_UP_PIN);
  }
  if (volDown) {
    turnVolume(VOL_DOWN_PIN);
  }
}

void togglePower() {
  if (powerOn) {
    digitalWrite(POWER_PIN, LOW);
    powerOn = false;
  } else {
    digitalWrite(POWER_PIN, HIGH);
    powerOn = true;
  }
}

void turnVolume(int pin) {
  if (powerOn) {
    digitalWrite(pin, HIGH);
    delay(40); // knob coarseness
    digitalWrite(pin, LOW);
    delay(30); // knob slowness
    volUp = false;
    volDown = false;
  }
}
