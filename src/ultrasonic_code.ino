/*
Ultrasonic Distance Warning System

Features:
- HC-SR04 ultrasonic distance measurement
- LED brightness adjusts based on ambient light
- Buzzer frequency changes based on distance (close distance = more buzzing)
- Serial monitoring

Author: Kush Suthar
*/

#define TRIG_PIN 9
#define ECHO_PIN 10
#define BUZZER_PIN 6
#define LED1_PIN 5
#define LED2_PIN 3
#define LDR_PIN A0

float distance;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
}

float getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.0343 / 2;
  return distance;
}

void setLEDs(bool state) {
  digitalWrite(LED1_PIN, state);
  digitalWrite(LED2_PIN, state);
}

void dimLEDs(int brightness) {
  analogWrite(LED1_PIN, brightness);
  analogWrite(LED2_PIN, brightness);
}

void loop() {
  distance = getDistanceCM();
  int ldrValue = analogRead(LDR_PIN);

  // darker = higher brightness
  int brightness = map(ldrValue, 150, 300, 255, 0);
  brightness = constrain(brightness, 0, 255);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Light: ");
  Serial.print(ldrValue);
  Serial.print(" | Brightness: ");
  Serial.println(brightness);

  if (distance > 50 || distance <= 0) {
    noTone(BUZZER_PIN);
    dimLEDs(brightness);
    delay(50);
  }
  else {
    int beepDelay = map(distance, 5, 50, 50, 500);
    beepDelay = constrain(beepDelay, 50, 500);
    setLEDs(HIGH);
    tone(BUZZER_PIN, 1000);
    delay(beepDelay);
    setLEDs(LOW);
    noTone(BUZZER_PIN);
    delay(beepDelay);
  }
}
