#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#define BUZZER_PIN 3  // Using Digital Pin 3 (PWM-capable pin on Arduino Nano)

// Create an instance of the ADXL345
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

int ADXL345 = 0x53;

float startX = 0, startY = 0, startZ = 0;
float startPitch = 0, startRoll = 0;
// float pitchThreshold = -20;
// float rollThreshold = 75;

float thresholdOffset = -0.3;

float pitchThreshold = -9.3 + thresholdOffset;
float rollThreshold = -3 + thresholdOffset;


float xThreshold = 5;
float yThreshold = -0.9;
float zThreshold = 3.7;

int msBeforeAlarm = 10000;
float updateTime = 500;
unsigned long lastRollTime = 0; // Track time when roll exceeds 130 degrees

enum DangerLevel {
  DANGER, // Roll above 130 degrees
  WARNING, // Roll between halfway (65 degrees) and full way (130 degrees)
  SAFE // Roll below 65 degrees
};

void setup() {
  Serial.begin(9600);
  Serial.println("ADXL345 Test");

  // Initialize the ADXL345
  if (!accel.begin(ADXL345)) {
    Serial.println("No ADXL345 detected ... Check your connections");
    while (1);
  }

  // Set the range to +/- 16g
  accel.setRange(ADXL345_RANGE_16_G);
  Serial.println("ADXL345 found!");
  
  // Give sensor time to stabilize
  delay(1000);

  // Read initial values
  sensors_event_t event;
  accel.getEvent(&event);
  startX = event.acceleration.x;
  startY = event.acceleration.y;
  startZ = event.acceleration.z;

  startPitch = atan2(-startX, sqrt(startY * startY + startZ * startZ)) * 180.0 / PI;
  startRoll  = atan2(startY, startZ) * 180.0 / PI;
  Serial.println("Baseline set!");

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
 // PC1 / Pin 36
}

float rollAvg = 0;
float rollSum = 0;
int rollCount = 0;

float pitchAvg = 0;
float pitchSum = 0;

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // float ax = event.acceleration.x - startX;
  // float ay = event.acceleration.y - startY;
  // float az = event.acceleration.z - startZ;

  float ax = event.acceleration.x;
  float ay = event.acceleration.y;
  float az = event.acceleration.z;


  float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  float roll  = atan2(ay, az) * 180.0 / PI;

  // float relPitch = pitch - startPitch;
  // float relRoll = roll - startRoll;
  float relPitch = pitch;
  float relRoll = roll;
  

  rollSum += roll;
  rollCount++;
  pitchSum += pitch;

  
  if (rollCount >= 10) {
    // Calc roll average
    rollAvg = rollSum / 10.0;
    Serial.print("Average Roll (last 10): ");
    Serial.print(rollAvg);
    Serial.println(" deg");

    // Reset for next batch
    rollSum = 0;
    rollCount = 0;

    // Calc pitch average
    pitchAvg = pitchSum / 10.0;
    Serial.print("Average Pitch (last 10): ");
    Serial.print(pitchAvg);
    Serial.println(" deg");

    // Reset for next batch
    pitchSum = 0;
  }

  // Print results
  Serial.print("Pitch: "); Serial.print(pitch); Serial.print(" deg");
  Serial.print(" Roll: "); Serial.print(roll); Serial.print(" deg");

  // Print the values
  Serial.print(" X: "); Serial.print(ax);
  Serial.print(" Y: "); Serial.print(ay);
  Serial.print(" Z: "); Serial.print(az);
  Serial.println();

  // bool tingeling = rollAvg > rollThreshold || pitch < pitchThreshold;
  bool tingeling2 = rollAvg <= rollThreshold || pitch <= pitchThreshold;
  
  if (!tingeling2) {
    if (lastRollTime == 0) { // First time exceeding threshold
      lastRollTime = millis();
    }

    // Check if the 5 seconds have passed
    unsigned long lastTime = millis() - lastRollTime;
    if (lastTime >= msBeforeAlarm) {
      setStatus(DANGER);
    } else if (lastTime >= (msBeforeAlarm / 2)) {
      setStatus(WARNING);
    }
  } else {
    lastRollTime = 0;
    setStatus(SAFE);
  }

  delay(updateTime);
}

int warningCount = 0;

void setStatus(DangerLevel level) {
  switch (level) {
    case DANGER:
      tone(BUZZER_PIN, 600, 100);
      delay(100);
      tone(BUZZER_PIN, 600, 100);
      warningCount = 0;
      break;

    case WARNING:
      warningCount++;
      int freq = 100 + warningCount * 30;
      tone(BUZZER_PIN, freq, 200);
      PORTC |= (1 << PC1);
      PORTC &= ~(1 << PC0);
      break;

    case SAFE:
      PORTC &= ~((1 << PC0) | (1 << PC1));
      warningCount = 0;
      break;
  }
}
