#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <SoftwareSerial.h>
#include <TFMPlus.h> 
#include <math.h>
#include "servo_control_page.h"

#define MATH_PI 3.14159

#define SERVOMIN  125 // minimum pulse length count (out of 4096)
#define SERVOMAX  575 // maximum pulse length count (out of 4096)

#define PIN_ENA 4 // PWM pin for motor 1 speed control
#define PIN_IN1 16
#define PIN_IN2 17

// Motor 2 settings
#define PIN_ENB 19 // PWM pin for motor 2 speed control
#define PIN_IN3 5
#define PIN_IN4 18

// Motor 3 settings
#define PIN_ENC 32 // PWM pin for motor 3 speed control
#define PIN_IN5 33
#define PIN_IN6 25

// Motor 4 settings
#define PIN_END 14 // PWM pin for motor 4 speed control
#define PIN_IN7 26
#define PIN_IN8 27

#define PAN_PIN 4
#define TILT_PIN 5

int panIncrement = 2;
unsigned long prevMillis = 0;
const unsigned long tiltInterval = 1000;

int panPos  = 0; 
int tiltPos = 0;
bool servoRunning = false;

Adafruit_PWMServoDriver board1;


////////////////////////////////////////////////////////////////////////////////
const char *ssid = "Your wifi name";
const char *password = "your wifi password";
////////////////////////////////////////////////////////////////////////////////


WebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);

SoftwareSerial tfSerial(35, 34); 
TFMPlus tfmP;                    

    int16_t tfDist = 0;   
    float x,y,z,panAngle,tiltAngle;

void handleRoot() {
  server.send(200, "text/html", servoControlPage);
}

void setServo() {
  String servoNumStr = server.arg("servo");
  String angleStr = server.arg("angle");

  if (servoNumStr.length() > 0 && angleStr.length() > 0) {
    int servoNum = servoNumStr.toInt();
    int angle = angleStr.toInt();

    if (servoNum >= 0 && servoNum <= 3 && angle >= 0 && angle <= 180) {
      int pwmValue = map(angle, 0, 180, SERVOMIN, SERVOMAX);
      board1.setPWM(servoNum, 0, pwmValue);

      server.send(200, "text/plain", "OK");
      return;
    }
  }

  server.send(400, "text/plain", "Invalid request parameters");
}

void controlMotor() {
  String action = server.arg("action");


  if(action == "W"){

  analogWrite(PIN_ENA, 255); // Motor 1 maximum speed
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);

  // Motor 2 forward
  analogWrite(PIN_ENB, 255); // Motor 2 maximum speed
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

  // Motor 3 forward
  analogWrite(PIN_ENC, 255); // Motor 3 maximum speed
  digitalWrite(PIN_IN5, HIGH);
  digitalWrite(PIN_IN6, LOW);

  // Motor 4 forward
  analogWrite(PIN_END, 255); // Motor 4 maximum speed
  digitalWrite(PIN_IN7, HIGH);
  digitalWrite(PIN_IN8, LOW);

  }else if(action == "S"){

  analogWrite(PIN_ENA, 255); // Motor 1 maximum speed
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);

  // Motor 2 forward
  analogWrite(PIN_ENB, 255); // Motor 2 maximum speed
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);

  // Motor 3 forward
  analogWrite(PIN_ENC, 255); // Motor 3 maximum speed
  digitalWrite(PIN_IN5, LOW);
  digitalWrite(PIN_IN6, HIGH);

  // Motor 4 forward
  analogWrite(PIN_END, 255); // Motor 4 maximum speed
  digitalWrite(PIN_IN7, LOW);
  digitalWrite(PIN_IN8, HIGH);

  }else if(action == "A"){
      // Motor 1 forward (Maintaining speed)
  analogWrite(PIN_ENA, 255);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);

  // Motor 2 reduced speed
  analogWrite(PIN_ENB, 200);  // Adjust the speed as needed
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

  // Motor 3 forward (Maintaining speed)
  analogWrite(PIN_ENC, 255);
  digitalWrite(PIN_IN5, HIGH);
  digitalWrite(PIN_IN6, LOW);

  // Motor 4 reduced speed
  analogWrite(PIN_END, 200);  // Adjust the speed as needed
  digitalWrite(PIN_IN7, HIGH);
  digitalWrite(PIN_IN8, LOW);

  }else if(action == "D"){

      // Motor 1 reduced speed
  analogWrite(PIN_ENA, 200);  // Adjust the speed as needed
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);

  // Motor 2 forward (Maintaining speed)
  analogWrite(PIN_ENB, 255);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, HIGH);

  // Motor 3 reduced speed
  analogWrite(PIN_ENC, 200);  // Adjust the speed as needed
  digitalWrite(PIN_IN5, HIGH);
  digitalWrite(PIN_IN6, LOW);

  // Motor 4 forward (Maintaining speed)
  analogWrite(PIN_END, 255);
  digitalWrite(PIN_IN7, HIGH);
  digitalWrite(PIN_IN8, LOW);

  }else{
        digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
    digitalWrite(PIN_IN3, LOW);
    digitalWrite(PIN_IN4, LOW);
    digitalWrite(PIN_IN5, LOW);
    digitalWrite(PIN_IN6, LOW);
    digitalWrite(PIN_IN7, LOW);
    digitalWrite(PIN_IN8, LOW);
    analogWrite(PIN_ENA, 0);
    analogWrite(PIN_ENB, 0);
    analogWrite(PIN_ENC, 0);
    analogWrite(PIN_END, 0);
  }

 server.send(200, "text/plain", "OK");
}




void handleStart() {
  servoRunning = true;
  server.send(200, "text/plain", "Servo control started");
}

void handleStop() {
  servoRunning = false;
  server.send(200, "text/plain", "Servo control stopped");
}

void controlScanning(){
  panPos += panIncrement;
  if(panPos > 180 || panPos < 0){
    panPos = constrain(panPos, 0, 180);
    panIncrement *= -1;
    Serial.println("Pan position out of range!");
  }
  board1.setPWM(PAN_PIN, 0, map(panPos, 0, 180, SERVOMIN, SERVOMAX));

  unsigned long currentMillis = millis();
  if(currentMillis - prevMillis >= tiltInterval){
    prevMillis = currentMillis;
    tiltPos++;
    if(tiltPos > 180 || tiltPos < 0){
      tiltPos = 0;
      Serial.println("Tilt position reset!");
    }
    board1.setPWM(TILT_PIN, 0, map(tiltPos, 0, 180, SERVOMIN, SERVOMAX));
  }
  delay(10);

    static unsigned long prevLidarMillis = 0;
  if (currentMillis - prevLidarMillis >= 0.2) { 
    prevLidarMillis = currentMillis;

    int16_t tfTemp = 0;   

    if (tfmP.getData(tfDist)) { 
      Serial.print("Dist: ");
      Serial.print(tfDist);
      //String data = "Pan: " + String(panPos) + " | Tilt: " + String(tiltPos) + " | LiDAR Distance: " + String(tfDist) + "cm";
      //webSocket.broadcastTXT(data);
    } else {                 
      tfmP.printFrame();    
    }
  }

    static unsigned long prevBroadcastMillis = 0;
  if (currentMillis - prevBroadcastMillis >=0.2) { 
    prevBroadcastMillis = currentMillis;

if(tfDist >= 5 && tfDist <= 500){ 

 //Normalizing angles to ensure they are within a specified range (0 to 2π)
panAngle = fmod(panPos * MATH_PI / 180.0, MATH_PI);
if (panAngle < 0) {
    panAngle += MATH_PI; // Ensure positive angle
}

tiltAngle = fmod(tiltPos * MATH_PI / 180.0, MATH_PI);
if (tiltAngle < 0) {
    tiltAngle += MATH_PI; // Ensure positive angle
}


        //  panAngle =  (panPos * MATH_PI )/ 180;
        //  tiltAngle = (tiltPos * MATH_PI) / 180;

          x = tfDist * sin(tiltAngle) * sin(panAngle);
          y = tfDist * sin(tiltAngle) * cos(panAngle);
          z = tfDist * cos(tiltAngle);

          // x = tfDist*sin(panAngle)*cos(tiltAngle);
          // y = tfDist*sin(panAngle)*sin(tiltAngle);
          // z = tfDist*cos(panAngle);

  
    // Prepare and broadcast data
    String data = "Pan: " + String(x) + " | Tilt: " + String(y) + " | LiDAR Distance: " + String(z) + "cm";
    webSocket.broadcastTXT(data);
     }    
  }
}

void setup() {
  Serial.begin(115200);

  board1 = Adafruit_PWMServoDriver(); // Initializing the Adafruit_PWMServoDriver instance
  board1.begin();
  board1.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);

  pinMode(PIN_ENB, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);

  pinMode(PIN_ENC, OUTPUT);
  pinMode(PIN_IN5, OUTPUT);
  pinMode(PIN_IN6, OUTPUT);

  pinMode(PIN_END, OUTPUT);
  pinMode(PIN_IN7, OUTPUT);
  pinMode(PIN_IN8, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

/////////////////////////////////////////////////////////////////////////////////
//////////////////once connected to your wifi and your ip address displayed
//////////////////on the serial monitor, use that address in the "NODE app
////////////////////////////////////////////////////////////////////////////////
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
/////////////////////////////////////////////////////////////////////////////////

  server.on("/", HTTP_GET, handleRoot);
  server.on("/set_servo", HTTP_GET, setServo);
  server.on("/control_motor", HTTP_GET, controlMotor);
  server.on("/start", handleStart); 
  server.on("/stop", handleStop); 
  server.begin();
  webSocket.begin();

  tfSerial.begin(115200);  
  delay(10);
  tfmP.begin(&tfSerial);  
  delay(10);

   if (tfmP.sendCommand(SET_FRAME_RATE, FRAME_20)) {
    printf("%2uHz.\r\n", FRAME_20);
  } else {
    tfmP.printReply();
  }
}

void loop() {
  server.handleClient();
  if(servoRunning) {
    controlScanning();
  }
    webSocket.loop();
  //delay(10);
}
