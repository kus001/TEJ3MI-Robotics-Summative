/*
ESP32 Car Web Controller

Features:
- WiFi access point control
- Web-based remote controller
- Adjustable speed modes
- 4 motor control using H-bridge drivers

Author: Kush Suthar
*/

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "********";
const char* password = "********";

WebServer server(80);

// front drivers
int FM1A = 27;
int FM1B = 26;
int FM2A = 25;
int FM2B = 33;

// back drivers
int BM1A = 5;
int BM1B = 18;
int BM2A = 23;
int BM2B = 19;

int currentSpeed = 150;

void setupPWM() {
  ledcAttach(FM1A, 1000, 8);
  ledcAttach(FM2A, 1000, 8);
  ledcAttach(BM1A, 1000, 8);
  ledcAttach(BM2A, 1000, 8);
}

// motor drivers
void driveF(int pinA, int pinB) {
  ledcWrite(pinA, currentSpeed);
  digitalWrite(pinB, LOW);
}

void driveB(int pinA, int pinB) {
  ledcAttach(pinB, 1000, 8);
  ledcWrite(pinA, 0);
  ledcWrite(pinB, currentSpeed);
}


void motorOff(int pinA, int pinB) {
  ledcWrite(pinA, 0);
  ledcWrite(pinB, 0);
}


// motor driving
void forward() {
  driveF(FM1A, FM1B);
  driveF(FM2A, FM2B);
  driveF(BM1A, BM1B);
  driveF(BM2A, BM2B);
}

void back() {
  driveB(FM1A, FM1B);
  driveB(FM2A, FM2B);
  driveB(BM1A, BM1B);
  driveB(BM2A, BM2B);
}

void right() {
  ledcWrite(FM2A, currentSpeed); digitalWrite(FM1B, LOW);
  ledcWrite(BM1A, currentSpeed); digitalWrite(BM2B, LOW);
}

void left() {
  ledcWrite(FM1A, currentSpeed); digitalWrite(FM2A, LOW);
  ledcWrite(BM2A, currentSpeed); digitalWrite(BM1B, LOW);
}

void stopMotors() {
  motorOff(FM1A, FM1B);
  motorOff(FM2A, FM2B);
  motorOff(BM1A, BM1B);
  motorOff(BM2A, BM2B);
}

// webpage
String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta charset="UTF-8">
<title>ESP32 Car</title>
<style>
body {
  margin:0;
  font-family:Arial;
  background:#111;
  color:white;
  text-align:center;
}
h2 { margin-top:15px; }
.grid {
  display:grid;
  grid-template-columns:100px 100px 100px;
  grid-template-rows:100px 100px 100px;
  justify-content:center;
  gap:10px;
  margin-top:30px;
}
.btn {
  background:#333;
  border:none;
  color:white;
  font-size:28px;
  border-radius:15px;
  touch-action:manipulation;
}
.btn:active { background:#00ff88; color:black; }
.speed-btns {
  margin-top:20px;
  display:flex;
  justify-content:center;
  gap:10px;
}
.spd {
  background:#333;
  border:none;
  color:white;
  font-size:16px;
  padding:12px 24px;
  border-radius:12px;
  touch-action:manipulation;
}
.spd.active { background:#00ff88; color:black; }
</style>
</head>
<body>
<h2>CAR CONTROL</h2>

<div class="speed-btns">
  <button class="spd" id="slow" onclick="setSpd('slow')">SLOW</button>
  <button class="spd active" id="medium" onclick="setSpd('medium')">MED</button>
  <button class="spd" id="fast" onclick="setSpd('fast')">FAST</button>
</div>

<div class="grid">
  <div></div>
  <button class="btn"
    ontouchstart="send('forward')" onmousedown="send('forward')"
    onmouseup="send('stop')" ontouchend="send('stop')">↑</button>
  <div></div>
  <button class="btn"
    ontouchstart="send('left')" onmousedown="send('left')"
    onmouseup="send('stop')" ontouchend="send('stop')">←</button>
  <button class="btn" onclick="send('stop')">STOP</button>
  <button class="btn"
    ontouchstart="send('right')" onmousedown="send('right')"
    onmouseup="send('stop')" ontouchend="send('stop')">→</button>
  <div></div>
  <button class="btn"
    ontouchstart="send('back')" onmousedown="send('back')"
    onmouseup="send('stop')" ontouchend="send('stop')">↓</button>
  <div></div>
</div>

<script>
function send(cmd) { fetch('/' + cmd); }
function setSpd(s) {
  fetch('/speed/' + s);
  document.querySelectorAll('.spd').forEach(b => b.classList.remove('active'));
  document.getElementById(s).classList.add('active');
}
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html; charset=utf-8", page);
}

void setup() {
  Serial.begin(115200);

  pinMode(FM1B, OUTPUT);
  pinMode(FM2B, OUTPUT);
  pinMode(BM1B, OUTPUT);
  pinMode(BM2B, OUTPUT);

  setupPWM();
  stopMotors();

  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/forward", []() { forward(); server.send(200, "text/plain", "ok"); });
  server.on("/back",    []() { back();    server.send(200, "text/plain", "ok"); });
  server.on("/left",    []() { left();    server.send(200, "text/plain", "ok"); });
  server.on("/right",   []() { right();   server.send(200, "text/plain", "ok"); });
  server.on("/stop",    []() { stopMotors(); server.send(200, "text/plain", "ok"); });

  server.on("/speed/slow",   []() { currentSpeed = 80;  server.send(200, "text/plain", "ok"); });
  server.on("/speed/medium", []() { currentSpeed = 150; server.send(200, "text/plain", "ok"); });
  server.on("/speed/fast",   []() { currentSpeed = 255; server.send(200, "text/plain", "ok"); });

  server.begin();
}

void loop() {
  server.handleClient();
}
