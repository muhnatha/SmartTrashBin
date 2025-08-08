#include <Arduino.h>
#include "ultrasonic.h"

const int FULL_THRESHOLD_CM = 10;  
const int INPUT_DETECT_CM = 10;     

void setup() {
  Serial.begin(9600);
  ultrasonicSetup(); 
}

void loop() {
    // Deteksi sampah masuk
  long dIn = readDistanceCM(TRIG_IN, ECHO_IN);
  if (dIn < INPUT_DETECT_CM) {
    digitalWrite(LED_PIN, HIGH);    
    Serial.println(" | Status: Barang Masuk");
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
