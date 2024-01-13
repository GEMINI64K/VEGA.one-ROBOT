#ifndef SERVO_CONTROL_PAGE_H
#define SERVO_CONTROL_PAGE_H

const char servoControlPage[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
<head>
  <title>Four Servo Control</title>
 <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 20px;
    }
    h1 {
      margin-bottom: 20px;
    }
    h2 {
      margin-top: 30px;
    }
    button {
      padding: 10px 20px;
      margin: 5px;
      font-size: 16px;
      border-radius: 5px;
      cursor: pointer;
      background-color: #4CAF50;
      color: white;
      border: none;
    }
    button:hover {
      background-color: #45a049;
    }
    .down{
      margin-top: 20px;
      margin-buttom: 20px;
    }
  </style>
</head>
<body>

<h1>Servo Control</h1>

<!-- Servo Buttons -->
<h2>Angles</h2>
<button onclick="setServoPosition(90)">90</button>
<button onclick="setServoPosition(120)">120</button>
<button onclick="setServoPosition(150)">150</button>

<!-- DC Motor Controls using WASD and steering -->
<h2>DC Motor Control</h2>
<div>
  <button onclick="controlMotor('W')">Forward</button>
</div>
<div>
  <button onclick="controlMotor('A')">Left</button>
  <button onclick="controlMotor('S')">Backward</button>
  <button onclick="controlMotor('D')">Right</button>
</div>
<div class="down">
  <button onclick="controlMotor('O')">stop</button>
</div>

<button onclick="startServoControl()">Start Servo Control</button>
<button onclick="stopServoControl()">Stop Servo Control</button>



<script>
document.addEventListener('keydown', function(event) {
  if (event.key === 'w' || event.key === 'W') {
    controlMotor('W');
  } else if (event.key === 'a' || event.key === 'A') {
    controlMotor('A');
  } else if (event.key === 's' || event.key === 'S') {
    controlMotor('S');
  } else if (event.key === 'd' || event.key === 'D') {
    controlMotor('D');
  } else if (event.key === 'o' || event.key === 'O'){
    contorlMOtor('O');
  }
});


function setServoPosition(angle) {
  var xhr = new XMLHttpRequest();

  // Function to create and send XMLHttpRequest
  function sendRequest(servo, angle) {
    xhr = new XMLHttpRequest();
    xhr.open("GET", `set_servo?servo=${servo}&angle=${angle}`, true);
    xhr.onerror = function () {
      console.error('Error sending request.');
    };
    xhr.send();
  }

  // Define servo positions based on angle
  var servoAngles = [angle, angle, angle, angle];
  if (angle != 90) {
    servoAngles[2] = angle - 60;
    servoAngles[3] = angle - 60;

    if (angle == 150) {
      servoAngles[2] = angle - 120;
      servoAngles[3] = angle - 120;
    }
  }

  // Loop through servos and angles, and send requests with delay
  var delay = 100; // Adjust delay in milliseconds
  for (let i = 0; i < servoAngles.length; i++) {
    setTimeout(() => {
      sendRequest(i, servoAngles[i]);
    }, i * delay);
  }
}

function controlMotor(action) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", `control_motor?action=${action}`, true);
  xhr.onerror = function () {
    console.error('Error sending request.');
  };
  xhr.send();
}

function startServoControl(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/start", true);
  xhr.onload = function() {
    if (xhr.status === 200) {
      console.log("Servo control started");
    }
  };
  xhr.send();
}

function stopServoControl(){  // Corrected function name
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/stop", true);
  xhr.onload = function() {
    if (xhr.status === 200) {
      console.log("Servo control stopped");
    }
  };
  xhr.send();
}


</script>

</body>
</html>

)=====";

#endif
