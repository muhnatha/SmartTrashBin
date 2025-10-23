// #include <Arduino.h>
// #include <Servo.h>
// #include "ultrasonic.h"

// // Konstanta jarak dan waktu
// const int FULL_THRESHOLD_CM = 10;  
// const int ORIGIN_DISTANCE_CM = 35; 
// const int THERE_IS_OBJECT = 5;    
// const unsigned long TIMEOUT_MS = 2000;

// // Definisi pin servo
// #define SERVO_RECYCLE 6   // Servo kiri (recycle)
// #define SERVO_B3 10       // Servo kanan (B3)
// #define SERVO_ORGANIC 9   // Servo tengah (organic)

// // Objek servo
// Servo servoRecycle;
// Servo servoB3;
// Servo servoOrganic;

// // Variabel kontrol servo
// unsigned long servoStartTime = 0;
// bool servoActive = false;
// int activeCommand = -1; // 0=recycle, 1=organic, 2=B3

// // Setup awal servo
// void servoSetup() {
//   servoRecycle.attach(SERVO_RECYCLE);
//   servoB3.attach(SERVO_B3);
//   servoOrganic.attach(SERVO_ORGANIC);

//   // Pastikan semua servo tertutup di awal
//   servoRecycle.write(0);
//   servoB3.write(0);
//   servoOrganic.write(0);
// }

// // Logika gerakan servo berdasarkan perintah
// void servoLogic(int command) {
//   servoStartTime = millis();
//   servoActive = true;
//   activeCommand = command;

//   switch (command) {
//     case 0: // Sampah Recycle
//       Serial.println(">> RECYCLABLE detected");
//       servoRecycle.write(90);
//       delay(500); // beri jeda sedikit antar servo
//       servoOrganic.write(90);
//       break;

//     case 1: // Sampah Organic
//       Serial.println(">> ORGANIC detected");
//       servoOrganic.write(90);
//       break;

//     case 2: // Sampah B3 (Battery)
//       Serial.println(">> B3 detected");
//       servoB3.write(90);
//       delay(500);
//       servoRecycle.write(90);
//       break;

//     default:
//       servoActive = false;
//       activeCommand = -1;
//       break;
//   }
// }

// // Mengembalikan servo ke posisi semula setelah 3 detik
// void updateServo() {
//   if (servoActive && (millis() - servoStartTime >= 3000)) {
//     switch (activeCommand) {
//       case 0: // Recycle
//         servoOrganic.write(0);
//         delay(500);
//         servoRecycle.write(0);
//         break;

//       case 1: // Organic
//         servoOrganic.write(0);
//         break;

//       case 2: // B3
//         servoRecycle.write(0);
//         delay(500);
//         servoB3.write(0);
//         break;
//     }

//     servoActive = false;
//     activeCommand = -1;
//   }
// }

// void setup() {
//   Serial.begin(9600);
//   ultrasonicSetup();
//   servoSetup();
//   Serial.println("System Ready...");
// }

// void loop() {
//   // Deteksi sampah masuk
//   long distance = readDistanceCM(TRIG_IN, ECHO_IN);
//   if ((distance > 0) && (distance < ORIGIN_DISTANCE_CM) && 
//       ((ORIGIN_DISTANCE_CM - distance) > THERE_IS_OBJECT)) {
//     digitalWrite(LED_PIN, HIGH);    
//     Serial.write('D'); // Kirim sinyal deteksi ke Raspberry

//     unsigned long startTime = millis();
//     while (Serial.available() == 0) {
//       if (millis() - startTime >= 5000) {
//         break; // Timeout 5 detik menunggu perintah dari Python
//       }
//     }

//     int command = Serial.read(); 
//     if (command != -1) {
//       servoLogic(command); // Jalankan logika servo
//     }

//     digitalWrite(LED_PIN, LOW);
//   }

//   updateServo();
//   delay(100);
// }


// // ----------------------- Kode Test dari Serial Monitor
// #include <Arduino.h>
// #include <Servo.h>

// // Definisi pin servo
// #define SERVO_RECYCLE 6   // Servo kiri (recycle)
// #define SERVO_B3 10       // Servo kanan (B3)
// #define SERVO_ORGANIC 9   // Servo tengah (organic)

// // Objek servo
// Servo servoRecycle;
// Servo servoB3;
// Servo servoOrganic;

// // Variabel kontrol
// unsigned long servoStartTime = 0;
// bool servoActive = false;
// int activeCommand = -1; // 0=recycle, 1=organic, 2=B3

// // Setup awal servo
// void servoSetup() {
//   servoRecycle.attach(SERVO_RECYCLE);
//   servoB3.attach(SERVO_B3);
//   servoOrganic.attach(SERVO_ORGANIC);

//   servoRecycle.write(0);
//   servoB3.write(0);
//   servoOrganic.write(0);
//   delay(500);
// }

// // Logika gerakan servo berdasarkan perintah
// void servoLogic(int command) {
//   servoStartTime = millis();
//   servoActive = true;
//   activeCommand = command;

//   switch (command) {
//     case 0: // Recycle
//       Serial.println(">> RECYCLABLE detected");
//       servoRecycle.write(90);
//       delay(500);
//       servoOrganic.write(90);
//       break;

//     case 1: // Organic
//       Serial.println(">> ORGANIC detected");
//       servoOrganic.write(90);
//       break;

//     case 2: // B3
//       Serial.println(">> B3 detected");
//       servoB3.write(90);
//       delay(500);
//       servoRecycle.write(90);
//       break;

//     default:
//       Serial.println(">> INVALID command");
//       servoActive = false;
//       activeCommand = -1;
//       break;
//   }
// }

// // Mengembalikan servo ke posisi semula setelah 3 detik
// void updateServo() {
//   if (servoActive && (millis() - servoStartTime >= 3000)) {
//     switch (activeCommand) {
//       case 0: // Recycle
//         servoOrganic.write(0);
//         delay(500);
//         servoRecycle.write(0);
//         break;

//       case 1: // Organic
//         servoOrganic.write(0);
//         break;

//       case 2: // B3
//         servoRecycle.write(0);
//         delay(500);
//         servoB3.write(0);
//         break;
//     }

//     servoActive = false;
//     activeCommand = -1;
//     Serial.println(">> Servo returned to initial position");
//   }
// }

// void setup() {
//   Serial.begin(9600);
//   servoSetup();
//   Serial.println("=== SERVO TEST MODE ===");
//   Serial.println("Input command:");
//   Serial.println("0 = Recycle");
//   Serial.println("1 = Organic");
//   Serial.println("2 = B3 (Battery)");
//   Serial.println("========================");
// }

// void loop() {
//   // Cek input dari Serial Monitor
//   if (Serial.available() > 0) {
//     char input = Serial.read(); // baca karakter langsung
//     if (input == '0') servoLogic(0);
//     else if (input == '1') servoLogic(1);
//     else if (input == '2') servoLogic(2);
//   }

//   updateServo();
// }
