// ==================== SMART TRASH BIN SYSTEM ====================

#include <Arduino.h>
#include <Servo.h>
#include "ultrasonic.h"

// ==================== Konstanta Ultrasonic ====================
const int FULL_THRESHOLD_CM = 10;  
const int ORIGIN_DISTANCE_CM = 35; 
const int THERE_IS_OBJECT = 5;     
const unsigned long TIMEOUT_MS = 2000;

// ==================== Definisi Pin Servo ====================
#define SERVO_RECYCLE 6   // Servo kiri (Recycle)
#define SERVO_ORGANIC 9   // Servo tengah (Organic)
#define SERVO_B3 10       // Servo kanan (B3)

// ==================== Objek Servo ====================
Servo servoRecycle;
Servo servoOrganic;
Servo servoB3;

// ==================== Posisi Default Servo ====================
const int POS_RECYCLE_DEFAULT = 120;
const int POS_ORGANIC_DEFAULT = 115;
const int POS_B3_DEFAULT = 180;

// ==================== Variabel Kendali Servo ====================
unsigned long servoStartTime = 0;
bool servoActive = false;
int activeCommand = -1; // 0=Recycle, 1=Organic, 2=B3

// ==================== Setup Servo ====================
void servoSetup() {
  servoRecycle.attach(SERVO_RECYCLE);
  servoOrganic.attach(SERVO_ORGANIC);
  servoB3.attach(SERVO_B3);

  // Posisi awal semua servo
  servoRecycle.write(POS_RECYCLE_DEFAULT);
  servoOrganic.write(POS_ORGANIC_DEFAULT);
  servoB3.write(POS_B3_DEFAULT);

  delay(500);
}

// ==================== Gerakan Servo Berdasarkan Kategori ====================
void servoLogic(int command) {
  servoStartTime = millis();
  servoActive = true;
  activeCommand = command;

  switch (command) {
    case 0: // === RECYCLE ===
      Serial.println(">> RECYCLABLE detected");
      servoRecycle.write(30);          // servo kiri buka
      delay(300);
      servoOrganic.write(20);          // servo tengah bantu buka
      break;

    case 1: // === ORGANIC ===
      Serial.println(">> ORGANIC detected");
      servoOrganic.write(20);          // servo tengah buka
      break;

    case 2: // === B3 (BATTERY) ===
      Serial.println(">> B3 detected");
      // 1️⃣ Servo B3 buka ke 100 derajat
      servoB3.write(100);
      delay(500);
      // 2️⃣ Servo Organic buka
      servoOrganic.write(20);
      delay(500);
      // 3️⃣ Servo Organic kembali
      servoOrganic.write(POS_ORGANIC_DEFAULT);
      delay(500);
      // 4️⃣ Servo B3 kembali perlahan
      for (int angle = 100; angle <= 150; angle++) {
        servoB3.write(angle);
        delay(15);
      }
      delay(500);
      break;

    default:
      Serial.println(">> INVALID command");
      servoActive = false;
      activeCommand = -1;
      break;
  }
}

// ==================== Kembalikan Servo ke Posisi Awal ====================
void updateServo() {
  if (servoActive && (millis() - servoStartTime >= 3000)) {
    switch (activeCommand) {
      case 0:
        servoOrganic.write(POS_ORGANIC_DEFAULT);
        delay(300);
        servoRecycle.write(POS_RECYCLE_DEFAULT);
        break;
      case 1:
        servoOrganic.write(POS_ORGANIC_DEFAULT);
        break;
      case 2:
        servoRecycle.write(POS_RECYCLE_DEFAULT);
        delay(300);
        servoB3.write(POS_B3_DEFAULT);
        break;
    }
    servoActive = false;
    activeCommand = -1;
    Serial.println(">> Servo returned to initial position");
  }
}

// ==================== Setup Awal ====================
void setup() {
  Serial.begin(115200);
  ultrasonicSetup();     // dari file ultrasonic.h
  servoSetup();          

  Serial.println("=== SMART TRASH BIN SYSTEM ===");
  Serial.println("Menunggu deteksi objek dari ultrasonic...");
  Serial.println("Raspi akan mengirim 0=Recycle, 1=Organic, 2=B3 setelah menerima huruf 'D'");
  Serial.println("==============================================");
}

// ==================== Loop Utama ====================
void loop() {
  // ==== 1️⃣ Cek apakah ada objek di depan sensor ====
  long distance = readDistanceCM(TRIG_IN, ECHO_IN);

  if ((distance > 0) && (distance < ORIGIN_DISTANCE_CM) && ((ORIGIN_DISTANCE_CM - distance) > THERE_IS_OBJECT)) {
    Serial.write('D');  // kirim sinyal ke Raspberry Pi
    Serial.println(" >> Object detected, sending 'D' to Raspi");

    // Tunggu Raspi kirim kategori (0, 1, atau 2)
    unsigned long startTime = millis();
    while (Serial.available() == 0) {
      if (millis() - startTime >= 5000) {
        Serial.println(">> Timeout: no response from Raspi");
        break;
      }
    }

    if (Serial.available() > 0) {
      char input = Serial.read();
      if (input == '0') servoLogic(0);
      else if (input == '1') servoLogic(1);
      else if (input == '2') servoLogic(2);
      else Serial.println(">> Invalid input from Raspi!");
    }
  }

  // ==== 2️⃣ Perbarui posisi servo jika waktu sudah habis ====
  updateServo();

  delay(200); // sedikit jeda untuk stabilisasi
}


//-------- SERIAL MONITOR TEST MODE FOR SERVOS --------
// #include <Arduino.h>
// #include <Servo.h>

// // === Definisi pin servo ===
// #define SERVO_RECYCLE 6   // Servo kiri (Recycle)
// #define SERVO_ORGANIC 9   // Servo tengah (Organic)
// #define SERVO_B3 10       // Servo kanan (B3)

// // === Objek servo ===
// Servo servoRecycle;
// Servo servoOrganic;
// Servo servoB3;

// // === Posisi default servo ===
// const int POS_RECYCLE_DEFAULT = 120;
// const int POS_ORGANIC_DEFAULT = 115;
// const int POS_B3_DEFAULT = 180;

// // === Variabel kontrol ===
// unsigned long servoStartTime = 0;
// bool servoActive = false;
// int activeCommand = -1; // 0=recycle, 1=organic, 2=B3

// // ------------------ Setup Awal Servo ------------------
// void servoSetup() {
//   servoRecycle.attach(SERVO_RECYCLE);
//   servoOrganic.attach(SERVO_ORGANIC);
//   servoB3.attach(SERVO_B3);

//   // Posisi awal semua servo
//   servoRecycle.write(POS_RECYCLE_DEFAULT);
//   servoOrganic.write(POS_ORGANIC_DEFAULT);
//   servoB3.write(POS_B3_DEFAULT);

//   delay(500);
// }

// // ------------------ Gerakan Berdasarkan Input ------------------
// void servoLogic(int command) {
//   servoStartTime = millis();
//   servoActive = true;
//   activeCommand = command;

//   switch (command) {
//     case 0: // === RECYCLE ===
//       Serial.println(">> RECYCLABLE detected");
//       servoRecycle.write(30);          // servo pin 6 buka
//       delay(300);
//       servoOrganic.write(20);          // servo pin 9 buka
//       break;

//     case 1: // === ORGANIC ===
//       Serial.println(">> ORGANIC detected");
//       servoOrganic.write(20);          // servo pin 9 buka
//       break;

//     case 2: // === B3 (BATTERY) ===
//     Serial.println(">> B3 detected");

//     // 1️⃣ Servo B3 buka ke 100 derajat (cepat)
//     servoB3.write(100);
//     delay(500);

//     // 2️⃣ Servo Organic buka ke 20 derajat (cepat)
//     servoOrganic.write(20);
//     delay(500);

//     // 3️⃣ Servo Organic kembali ke 115 derajat (cepat)
//     servoOrganic.write(115);
//     delay(500);

//     // 4️⃣ Servo B3 kembali perlahan ke 150 derajat (posisi awal)
//     for (int angle = 100; angle <= 150; angle++) {
//     servoB3.write(angle);
//     delay(15); // Ubah untuk atur kecepatan: kecil = lebih cepat
//     }

//   delay(500);
//   break;

//     default:
//       Serial.println(">> INVALID command");
//       servoActive = false;
//       activeCommand = -1;
//       break;
//   }
// }

// // ------------------ Kembalikan Servo ke Posisi Awal ------------------
// void updateServo() {
//   if (servoActive && (millis() - servoStartTime >= 3000)) {
//     switch (activeCommand) {
//       case 0: // inorganic
//         servoOrganic.write(POS_ORGANIC_DEFAULT);   // kembali ke 115
//         delay(300);
//         servoRecycle.write(POS_RECYCLE_DEFAULT);   // kembali ke 120
//         break;

//       case 1: // Organic
//         servoOrganic.write(POS_ORGANIC_DEFAULT);   // kembali ke 115
//         break;

//       case 2: // B3
//         servoRecycle.write(POS_RECYCLE_DEFAULT);   // servo 6 kembali ke 120
//         delay(300);
//         servoB3.write(POS_B3_DEFAULT);             // servo 10 kembali ke 180
//         break;
//     }

//     servoActive = false;
//     activeCommand = -1;
//     Serial.println(">> Servo returned to initial position");
//   }
// }

// // ------------------ Setup ------------------
// void setup() {
//   Serial.begin(115200);
//   servoSetup();

//   Serial.println("=== SERVO TEST MODE (MANUAL INPUT) ===");
//   Serial.println("Input command:");
//   Serial.println("0 = Recycle");
//   Serial.println("1 = Organic");
//   Serial.println("2 = B3 (Battery)");
//   Serial.println("=======================================");
// }

// // ------------------ Loop Utama ------------------
// void loop() {
//   // Baca input dari Serial Monitor
//   if (Serial.available() > 0) {
//     char input = Serial.read();

//     if (input == '0') servoLogic(0);
//     else if (input == '1') servoLogic(1);
//     else if (input == '2') servoLogic(2);
//     else {
//       Serial.println(">> Invalid input! Please enter 0, 1, or 2.");
//     }
//   }

//   updateServo();
// }