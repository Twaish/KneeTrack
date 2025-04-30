#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// Create an instance of the ADXL345
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

int ADXL345 = 0x53;

float startX = 0, startY = 0, startZ = 0;
float startPitch = 0, startRoll = 0;
int msBeforeAlarm = 5000;
float updateTime = 200;
unsigned long lastRollTime = 0; // Track time when roll exceeds 130 degrees

// #define LED_DANGER_PIN A0  // PORTC.0
// #define LED_WARNING_PIN A1 // PORTC.1

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

  pinMode(A8, OUTPUT); // PC0 / Pin 37
  pinMode(A9, OUTPUT); // PC1 / Pin 36
  // pinMode(LED_DANGER_PIN, OUTPUT);
  // pinMode(LED_WARNING_PIN, OUTPUT);
}

float rollSum = 0;
int rollCount = 0;
float rollAvg = 0;

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  float ax = event.acceleration.x - startX;
  float ay = event.acceleration.y - startY;
  float az = event.acceleration.z - startZ;

  float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  float roll  = atan2(ay, az) * 180.0 / PI;

  float relPitch = pitch - startPitch;
  float relRoll = roll - startRoll;
  
  rollSum += roll;
  rollCount++;
  
  if (rollCount >= 10) {
    rollAvg = rollSum / 10.0;
    Serial.print("Average Roll (last 10): ");
    Serial.print(rollAvg);
    Serial.println(" deg");

    // Reset for next batch
    rollSum = 0;
    rollCount = 0;
  }

  // Print results
  Serial.print("Pitch: "); Serial.print(pitch); Serial.print(" deg");
  Serial.print(" Roll: "); Serial.print(roll); Serial.print(" deg");

  // Print the values
  Serial.print(" X: "); Serial.print(ax);
  Serial.print(" Y: "); Serial.print(ay);
  Serial.print(" Z: "); Serial.print(az);
  Serial.println();

  // If roll exceeds 130, start the timer
  if (rollAvg > 130) {
    if (lastRollTime == 0) { // First time exceeding 130 degrees
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

void setStatus(DangerLevel level) {
  switch (level) {
    case DANGER:
      PORTC |= (1 << PC0);  // Turn on PC0
      PORTC &= ~(1 << PC1); // Turn off PC1
      break;
      
    case WARNING:
      PORTC |= (1 << PC1);  // Turn on PC1
      PORTC &= ~(1 << PC0); // Turn off PC0
      break;

    case SAFE:
      PORTC &= ~((1 << PC0) | (1 << PC1)); // Turn off both LEDs
      break;
  }
}
