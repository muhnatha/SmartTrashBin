#ifndef LISTEN_RASPI_H
#define LISTEN_RASPI_H

#include <Arduino.h>
#include "ServoControl.h" 

String raspiBuffer = "";  

inline void raspiSetup() {
  Serial.begin(9600);
}

inline void listenRaspi() {
  // Cek apakah ada data dari Raspberry Pi
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\n') {
      raspiBuffer.trim(); 

      int cmd = raspiBuffer.toInt();

      if (cmd == 0) {
        Serial.println("Organik terdeteksi → Servo 1");
        servoControl(1);
      } 
      else if (cmd == 1) {
        Serial.println("Anorganik terdeteksi → Servo 2");
        servoControl(2);
      } 
      else {
        Serial.print("Data tidak valid: ");
        Serial.println(raspiBuffer);
      }

      raspiBuffer = ""; 
    } else {
      raspiBuffer += c; 
    }
  }
}

#endif
