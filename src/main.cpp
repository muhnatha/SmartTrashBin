#include <Arduino.h>
#include "ultrasonic.h"

void setup() {
  Serial.begin(9600);
  ultrasonicSetup(); 
}

void loop() {
  long d1 = readDistanceCM(TRIG1, ECHO1);
  Serial.print("Sensor 1: ");
  Serial.print(d1);
  Serial.println(" cm");
  delay(1000);
}