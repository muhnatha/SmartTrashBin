#ifndef MY_SERVO_CONTROL_H
#define MY_SERVO_CONTROL_H

#include <Arduino.h>
#include <Servo.h>

#define SERVO1_PIN 11
#define SERVO2_PIN 12
#define SERVO3_PIN 13
#define SERVO4_PIN A0

Servo servo1, servo2, servo3, servo4;

unsigned long servoStartTime = 0;
int activeServo = 0;
bool servoMoving = false;

inline void servoSetup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);

  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
}

inline void servoControl(int inputNumber) {
  if (inputNumber < 1 || inputNumber > 4) return; 
  activeServo = inputNumber;
  servoStartTime = millis();
  servoMoving = true;

  switch (inputNumber) {
    case 1: servo1.write(90); break;
    case 2: servo2.write(90); break;
    case 3: servo3.write(90); break;
    case 4: servo4.write(90); break;
  }
}

inline void updateServo() {
  if (servoMoving && millis() - servoStartTime >= 3000) {
    switch (activeServo) {
      case 1: servo1.write(0); break;
      case 2: servo2.write(0); break;
      case 3: servo3.write(0); break;
      case 4: servo4.write(0); break;
    }
    servoMoving = false;
    activeServo = 0;
  }
}

#endif
