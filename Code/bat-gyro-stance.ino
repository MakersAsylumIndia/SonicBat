#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
const int buzzerPin = 4;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL for ESP32
  mpu.initialize();
  pinMode(buzzerPin, OUTPUT);

  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected.");
  } else {
    Serial.println("MPU6050 connection failed.");
  }
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert to Gs
  float y = ay / 16384.0;
  float z = az / 16384.0;

  // Calculate tilt angle in degrees (Y-Z plane)
  float angle = atan2(y, z) * 180 / PI;

  // Print raw readings and calculated angle
  Serial.print("AY: "); Serial.print(y, 2);
  Serial.print(" | AZ: "); Serial.print(z, 2);
  Serial.print(" | Angle: "); Serial.print(angle, 1);

  // Determine bat position
  if (angle < -20) {
    Serial.println(" --> BACKLIFT");
  } else if (angle > 20) {
    Serial.println(" --> FRONTLIFT");
  } else {
    Serial.println(" --> STATIONARY");
  }

  // Optional buzzer feedback if extreme motion detected
  if (abs(ax) > 10000 || abs(ay) > 10000) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  delay(200);
}
