#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include <Arduino.h>

//Box sensors
#define TRIG1 2
#define ECHO1 3
#define TRIG2 4
#define ECHO2 5
#define TRIG3 6
#define ECHO3 7

// Input sensor
#define TRIG_IN 11
#define ECHO_IN 12

// LED  
#define LED_PIN 8

inline void ultrasonicSetup() {
  // Box sensors
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT); pinMode(ECHO3, INPUT);

  // Input sensor
  pinMode(TRIG_IN, OUTPUT); pinMode(ECHO_IN, INPUT);

  // LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); 
}

inline long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration * 0.034 / 2;
}

#endif
