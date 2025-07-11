#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// Motor pins
const int MOTOR_LEFT = 5;
const int MOTOR_CENTER = 19;
const int MOTOR_RIGHT = 18;

// Trigger distance in millimeters
const int TRIGGER_DISTANCE = 2000; // 2000mm = 200cm

// Number of readings to detect direction
const int NUM_READINGS = 5;
int readings[NUM_READINGS];

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(MOTOR_LEFT, OUTPUT);
  pinMode(MOTOR_CENTER, OUTPUT);
  pinMode(MOTOR_RIGHT, OUTPUT);
  stopAllMotors();

  // Start sensor
  if (!lox.begin()) {
    Serial.println(F("VL53L0X not found! Check wiring."));
    while (1);
  }
  Serial.println("VL53L0X ready");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  // Take multiple readings
  for (int i = 0; i < NUM_READINGS; i++) {
    lox.rangingTest(&measure, false);
    if (measure.RangeStatus != 4) {
      readings[i] = measure.RangeMilliMeter;
    } else {
      readings[i] = 8190; // Out of range
    }
    delay(50); // Short delay between readings
  }

  // Analyze distance changes
  int leftSum = 0, rightSum = 0;
  for (int i = 1; i < NUM_READINGS / 2; i++) {
    leftSum += readings[i - 1] - readings[i];
    rightSum += readings[NUM_READINGS - i] - readings[NUM_READINGS - i - 1];
  }

  int avgDistance = readings[NUM_READINGS / 2];

  Serial.print("Avg Distance: ");
  Serial.print(avgDistance);
  Serial.print(" mm | LeftSum: ");
  Serial.print(leftSum);
  Serial.print(" | RightSum: ");
  Serial.println(rightSum);

  stopAllMotors();

  if (avgDistance < TRIGGER_DISTANCE) {
    // If ball approaching from left
    if (leftSum > 100) {
      digitalWrite(MOTOR_LEFT, HIGH);
    }
    // If ball approaching from right
    else if (rightSum > 100) {
      digitalWrite(MOTOR_RIGHT, HIGH);
    }
    // If coming straight
    else {
      digitalWrite(MOTOR_CENTER, HIGH);
    }
  }

  delay(100); // Loop delay
}

void stopAllMotors() {
  digitalWrite(MOTOR_LEFT, LOW);
  digitalWrite(MOTOR_CENTER, LOW);
  digitalWrite(MOTOR_RIGHT, LOW);
}