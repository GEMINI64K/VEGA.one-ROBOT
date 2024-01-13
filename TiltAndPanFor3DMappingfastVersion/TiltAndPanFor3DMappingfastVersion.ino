#include <ESP32Servo.h>

Servo tiltServo;
Servo panServo;

const int minPanPos = 0;     // Minimum pan position
const int maxPanPos = 180;   // Maximum pan position
const int minTiltPos = 0;    // Minimum tilt position
const int maxTiltPos = 180;  // Maximum tilt position

int panPos = minPanPos;      // Current pan position
int tiltPos = minTiltPos;    // Current tilt position

int panIncrement = 2;        // Increment value for pan movement
unsigned long prevMillis = 0; // Variable to store the previous time
const unsigned long tiltInterval = 1000; // Time interval for increasing tilt position (milliseconds)

void setup() {
  tiltServo.attach(25);  // Attach tilt servo to pin 13
  panServo.attach(26);   // Attach pan servo to pin 12

  tiltServo.write(tiltPos);  // Set initial tilt position
  panServo.write(panPos);    // Set initial pan position
  delay(500);                // Delay for initialization
}

void loop() {
  unsigned long currentMillis = millis(); // Get current time

  // Pan movement
  panPos += panIncrement;  // Increment pan position
  if (panPos > maxPanPos || panPos < minPanPos) {
    panPos = constrain(panPos, minPanPos, maxPanPos); // Limit pan position within range
    panIncrement *= -1; // Reverse the increment direction
    Serial.println("Pan position out of range!"); // Error message for out-of-range pan position
  }
  panServo.write(panPos);  // Move the pan servo
  delay(10);               // Delay for smoother movement

  // Increase tilt after every tiltInterval milliseconds
  if (currentMillis - prevMillis >= tiltInterval) {
    prevMillis = currentMillis; // Update the previous time

    tiltPos++; // Increase tilt position by 1 degree
    if (tiltPos > maxTiltPos || tiltPos < minTiltPos) {
      tiltPos = constrain(tiltPos, minTiltPos, maxTiltPos); // Limit tilt position within range
      Serial.println("Tilt position out of range!"); // Error message for out-of-range tilt position
    }
    tiltServo.write(tiltPos); // Move the tilt servo
  }
}
