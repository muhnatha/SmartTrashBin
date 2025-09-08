#include <Arduino.h>
#include "ultrasonic.h"
#include "ServoControl.h"

const int FULL_THRESHOLD_CM = 10;  
const int ORIGIN_DISTANCE_CM = 35; 
const int THERE_IS_OBJECT = 5;    
const unsigned long TIMEOUT_MS = 5000;

void setup() {
  Serial.begin(9600);
  ultrasonicSetup(); 
  servoSetup();
}

void loop() {
  // Deteksi sampah masuk
  long distance = readDistanceCM(TRIG_IN, ECHO_IN);
  if ((distance < (ORIGIN_DISTANCE_CM - THERE_IS_OBJECT))) {
    digitalWrite(LED_PIN, HIGH);    
    Serial.println("D");

    unsigned long startTime = millis(); 
    bool commandReceived = false;

    while ((millis() - startTime) < TIMEOUT_MS) {
      if (Serial.available() > 0) {
        int command = Serial.read(); 
        servoLogic(command);        
        commandReceived = true;    
        break;                       
      }
    }
  }
  else {
    digitalWrite(LED_PIN, LOW);   
    Serial.println(" | Status: Tidak Ada Barang");
  }

  long d1 = readDistanceCM(TRIG1, ECHO1);
  long d2 = readDistanceCM(TRIG2, ECHO2);
  long d3 = readDistanceCM(TRIG3, ECHO3);

  if (d1 < FULL_THRESHOLD_CM) {
    Serial.print("Box 1: PENUH ");
  }

  if (d2 < FULL_THRESHOLD_CM) {
    Serial.print(" | Box 2: PENUH ");
  }

  if (d3 < FULL_THRESHOLD_CM) {
    Serial.print(" | Box 3: PENUH");
  }

  delay(100);
}
