#include <AUnit.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define BUZZER_PIN 3

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
float pitchThreshold = -9.3;
float rollThreshold = -3;

enum DangerLevel {
  DANGER,
  WARNING,
  SAFE
};

DangerLevel currentLevel = SAFE;

void setStatus(DangerLevel level) {
  currentLevel = level;
}

// Function to calculate average
float calculateAverage(float* values, int count) {
  float sum = 0;
  for (int i = 0; i < count; i++) {
    sum += values[i];
  }
  return sum / count;
}

test(setStatusTest) {
  setStatus(DANGER);
  assertEqual(currentLevel, DANGER);

  setStatus(WARNING);
  assertEqual(currentLevel, WARNING);

  setStatus(SAFE);
  assertEqual(currentLevel, SAFE);
}

test(thresholdTest) {
  // Test with values below the threshold
  float roll = -4.0; // Below rollThreshold
  float pitch = -10.0; // Below pitchThreshold

  if (roll <= rollThreshold || pitch <= pitchThreshold) {
    setStatus(SAFE);
  }
  assertEqual(currentLevel, SAFE);

  // Test with values above the threshold
  roll = -2.0; // Above rollThreshold
  pitch = -8.0; // Above pitchThreshold

  if (roll > rollThreshold || pitch > pitchThreshold) {
    setStatus(DANGER);
  }
  assertEqual(currentLevel, DANGER);
}

test(averageCalculationTest) {
  float values[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
  float average = calculateAverage(values, 10);
  assertEqual(average, 55.0); // Expected average
}

test(sensorReadingTest) {
  // Simulate sensor readings
  float ax = 0.0;
  float ay = 0.0;
  float az = 1.0; // Simulating flat position

  float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  float roll = atan2(ay, az) * 180.0 / PI;

  assertEqual(pitch, 0.0); // Expected pitch
  assertEqual(roll, 0.0); // Expected roll
}

test(buzzerFunctionalityTest) {
  // Simulate buzzer behavior
  setStatus(DANGER);
  assertEqual(currentLevel, DANGER);
  // Here you would check if the buzzer is triggered (mocking may be needed)

  setStatus(WARNING);
  assertEqual(currentLevel, WARNING);
  // Check buzzer behavior for warning

  setStatus(SAFE);
  assertEqual(currentLevel, SAFE);
  // Check buzzer behavior for safe
}

void setup() {
  Serial.begin(9600);
  // Initialize the AUnit framework
  aunit::TestRunner::run();
}

void loop() {
  // No need to run anything in loop for tests
}
