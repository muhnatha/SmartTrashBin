#include <Arduino.h>
#include "ultrasonic.h"

void ultrasonicSetup() {
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
}

long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration * 0.034 / 2;
}
