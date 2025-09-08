#ifndef MY_SERVO_CONTROL_H
#define MY_SERVO_CONTROL_H

#include <Arduino.h>
#include <Servo.h>

// -- Define the pins for your servos --
#define SERVO1_PIN 11 // Pin for Organic servo
#define SERVO2_PIN 12 // Pin for Battery Waste servo

// Create servo objects
Servo servo1;
Servo servo2;

// Variables to manage the servo return timer
unsigned long servoStartTime = 0;
int activeServo = 0;      // 0=none, 1=servo1, 2=servo2
bool servoMoving = false;

inline void servoSetup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  // Start with both servo flaps closed (at 0 degrees)
  servo1.write(0);
  servo2.write(0);
}

inline void servoLogic(int command) {
  // command 0 = Organic -> move servo 1
  // command 2 = Battery -> move servo 2
  // command 1 = Inorganic -> do nothing

  if (command == 0) {
    activeServo = 1;      
    servoStartTime = millis(); 
    servoMoving = true;
    servo1.write(90);     
  } else if (command == 2) {
    activeServo = 2;      
    servoStartTime = millis(); 
    servoMoving = true;
    servo2.write(90);    
  }
}


inline void updateServo() {
  // Check if a servo is currently moving and if 2 seconds have passed
  if (servoMoving && (millis() - servoStartTime >= 2000)) {
    if (activeServo == 1) {
      servo1.write(0); // Close the organic flap
    } else if (activeServo == 2) {
      servo2.write(0); // Close the battery flap
    }
    // Reset the state variables
    servoMoving = false;
    activeServo = 0;
  }
}

#endif