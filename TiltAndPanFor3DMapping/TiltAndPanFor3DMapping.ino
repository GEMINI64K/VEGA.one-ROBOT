#include <ESP32Servo.h>

Servo tiltServo;
Servo panServo;

const int minPanPos = 0;     // Minimum pan position
const int maxPanPos = 180;   // Maximum pan position
const int minTiltPos = 0;    // Minimum tilt position
const int maxTiltPos = 180;  // Maximum tilt position

int panPos = minPanPos;      // Current pan position
int tiltPos = minTiltPos;    // Current tilt position

int panIncrement = 1;        // Increment value for pan movement
unsigned long prevMillis = 0; // Variable to store the previous time

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
  panServo.write(panPos);  // Move the pan servo
  delay(15);               // Delay for smoother movement

  // Check if pan reached its limits
  if (panPos >= maxPanPos || panPos <= minPanPos) {
    panIncrement *= -1; // Reverse the increment direction
  }

  // Increase tilt after every 2.7 seconds (2700 milliseconds)
  if (currentMillis - prevMillis >= 2700) {
    prevMillis = currentMillis; // Update the previous time
    
    tiltPos++; // Increase tilt position by 1 degree
    tiltPos = constrain(tiltPos, minTiltPos, maxTiltPos); // Limit tilt position
    tiltServo.write(tiltPos); // Move the tilt servo
  }
}
