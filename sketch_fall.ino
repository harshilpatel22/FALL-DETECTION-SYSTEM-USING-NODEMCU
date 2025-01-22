#define BLYNK_AUTH_TOKEN "wIpBlApcUSPAUNfVno3YZWLtUdFGDE6H"
#define BLYNK_TEMPLATE_ID "TMPL3PJR-C3bf"
#define BLYNK_TEMPLATE_NAME "Fall Detection"



#include <Blynk.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <BlynkSimpleEsp8266.h> // Include Blynk library




MPU6050 mpu(Wire);

// Blynk credentials
// Replace with your Blynk Auth Token
char ssid[] = "Ifone";              // Replace with your WiFi SSID
char pass[] = "Harshil2004";          // Replace with your WiFi Password

const float FALL_THRESHOLD = 2.5;
const int FALL_DURATION = 750;

float prevTotalAcc = 1.0; // Previous total acceleration
unsigned long fallStartTime = 0;
bool fallingDetected = false;

// BLE server name (not needed for Blynk)
#define SDA_PIN 4
#define SCL_PIN 5

void setup() {
  Serial.begin(115200);
 
  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
 
  Wire.begin(SDA_PIN, SCL_PIN);
 
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status != 0) {
    Serial.println(F("Could not connect to MPU6050"));
    delay(1000);
  }

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // gyro and accelero offsets
  Serial.println("Done!\n");
}

void loop() {
  Blynk.run(); // Keep the Blynk connection alive
  mpu.update();
 
  float accX = mpu.getAccX();
  float accY = mpu.getAccY();
  float accZ = mpu.getAccZ();
 
  // Calculate total acceleration
  float totalAcc = sqrt(accX * accX + accY * accY + accZ * accZ);
 
  // Detect sudden change in acceleration
  if (abs(totalAcc - prevTotalAcc)> FALL_THRESHOLD) {
    if (!fallingDetected) {
      fallingDetected = true;
      fallStartTime = millis();
    }
  } else if (fallingDetected && (millis() - fallStartTime > FALL_DURATION)) {
    Serial.println("Fall detected!");
    sendFallNotification();
    fallingDetected = false;
  }
 
  prevTotalAcc = totalAcc;
 
  delay(10); // Short delay for stability
}

// Function to send notification through Blynk app
void sendFallNotification() {
  Blynk.logEvent("fall_detect", "Fall detected! Check the person immediately.");
}
