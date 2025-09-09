#include <Arduino.h>
#include <Servo.h>
#include "ultrasonic.h"

// constants
const int FULL_THRESHOLD_CM = 10;  
const int ORIGIN_DISTANCE_CM = 35; 
const int THERE_IS_OBJECT = 5;    
const unsigned long TIMEOUT_MS = 2000;

// Define Servo pin
#define SERVO1_PIN 11 // Pin for Organic servo
#define SERVO2_PIN 12 // Pin for Battery Waste servo

// define servo objects and variables
Servo servo1;
Servo servo2;
unsigned long servoStartTime = 0;
int activeServo = 0; // 0=none, 1=servo1, 2=servo2
bool servoMoving = false;

// setup servo
void servoSetup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0); // Start with flap closed
  servo2.write(0); // Start with flap closed
}

// servo logic
void servoLogic(int command) {
  if (command == 0) {
    activeServo = 1;
    servoStartTime = millis();
    servoMoving = true;
    servo1.write(90); // Open organic flap
  } else if (command == 2) {
    activeServo = 2;
    servoStartTime = millis();
    servoMoving = true;
    servo2.write(90); // Open battery flap
  }
}

// restart servo
void updateServo() {
  if (servoMoving && (millis() - servoStartTime >= 2000)) {
    if (activeServo == 1) {
      servo1.write(0);
    } else if (activeServo == 2) {
      servo2.write(0);
    }
    servoMoving = false;
    activeServo = 0;
  }
}

void setup() {
  Serial.begin(9600);
  ultrasonicSetup();
  servoSetup(); 
}

void loop() {
  // Deteksi sampah masuk
  long distance = readDistanceCM(TRIG_IN, ECHO_IN);
  if ((distance > 0) && (distance < ORIGIN_DISTANCE_CM) && ((ORIGIN_DISTANCE_CM-distance) > THERE_IS_OBJECT)) {
    digitalWrite(LED_PIN, HIGH);    
    Serial.write('D');

    int command = Serial.read(); 
    servoLogic(command);        
    
    digitalWrite(LED_PIN, LOW);
  }
  updateServo();
  delay(100);
  //else {
  //  digitalWrite(LED_PIN, LOW);   
  //  Serial.println(" | Status: Tidak Ada Barang");
  //}

  //long d1 = readDistanceCM(TRIG1, ECHO1);
  //long d2 = readDistanceCM(TRIG2, ECHO2);
  //long d3 = readDistanceCM(TRIG3, ECHO3);

  //if (d1 < FULL_THRESHOLD_CM) {
  //  Serial.print("Box 1: PENUH ");
  //}
//
  //if (d2 < FULL_THRESHOLD_CM) {
  //  Serial.print(" | Box 2: PENUH ");
  //}
//
  //if (d3 < FULL_THRESHOLD_CM) {
  //  Serial.print(" | Box 3: PENUH");
  //}
//
  //delay(100);
}
