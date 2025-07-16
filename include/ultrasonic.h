#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define TRIG1 2
#define ECHO1 3

void ultrasonicSetup();
long readDistanceCM(int trigPin, int echoPin);

#endif