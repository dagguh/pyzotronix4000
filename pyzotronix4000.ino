#include <Arduino.h>
#define IR_INPUT_PIN 2
#define VOL_UP_PIN 3
#define VOL_DOWN_PIN 4
#define POWER_PIN 5
#define TOGGLE_PIN 6

#include "TinyIRReceiver.hpp"

bool powered = false;
bool poweringUp = false;
bool poweringDown = false;
bool volUp = false;
bool volDown = false;

void togglePower() {
  if (powered) {
    poweringDown = true;
    poweringUp = false;
  } else {
    poweringUp = true;
    poweringDown = false;
  }
}

void turnVolume(int pin) {
  if (powered) {
    digitalWrite(pin, HIGH);
    delay(40); // knob coarseness
    digitalWrite(pin, LOW);
    delay(30); // knob slowness
  }
}

struct Remote {
  const uint8_t powerToggle;
  const uint8_t volumeUp;
  const uint8_t volumeDown;

  bool receive(uint8_t aCommand, bool isRepeat) {
    if (powerToggle == aCommand && !isRepeat) {
      togglePower();
      return true;
    }
    if (volumeUp == aCommand) {
      volUp = true;
      return true;
    }
    if (volumeDown == aCommand) {
      volDown = true;
      return true;
    }
    return false;
  }
};

const Remote y04g0024 = {
  .powerToggle = 0,
  .volumeUp = 4,
  .volumeDown = 8
};

const Remote z906 = {
  .powerToggle = 0x80,
  .volumeUp = 0xAA,
  .volumeDown = 0x6A
};

Remote remotes[] = {z906, y04g0024};

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
  int remoteCount = sizeof(remotes) / sizeof(remotes[0]);
  for (int i = 0; i < remoteCount; i++) {
    bool handled = remotes[i].receive(aCommand, isRepeat);
    if (handled) {
      break;
    }
  }
}

void loop() {
  if (digitalRead(TOGGLE_PIN) == LOW) {
    delay(400); // debounce
    togglePower();
  }
  if (poweringUp) {
    digitalWrite(POWER_PIN, HIGH);
    powered = true;
    poweringUp = false;
  }
  if (poweringDown) {
    digitalWrite(POWER_PIN, LOW);
    powered = false;
    delay(5000); // apparently the DAC needs some time after powering down
    // would be nice to flash a LED to indicate this delay
    poweringDown = false;
  }
  if (volUp) {
    turnVolume(VOL_UP_PIN);
    volUp = false;
  }
  if (volDown) {
    turnVolume(VOL_DOWN_PIN);
    volDown = false;
  }
}
