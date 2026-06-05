#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// --- Motor A (Left Motor) Pins ---
const int enableLeftPin = 25;  // PWM pin (ENA)
const int motorLeftPin1 = 26;  // Direction (IN1)
const int motorLeftPin2 = 27;  // Direction (IN2)

// --- Motor B (Right Motor) Pins ---
const int enableRightPin = 13; // PWM pin (ENB)
const int motorRightPin1 = 14; // Direction (IN3)
const int motorRightPin2 = 12; // Direction (IN4)

// PWM Settings
const int pwmFreq = 5000;
const int pwmResolution = 8; // 0-255

// Function Prototypes (Fixes "not declared in this scope")
void updateMotors();
void stopMotors();

// BLE UUIDs (Nordic UART Service - Standard for Web BLE)
#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"

BLEServer *pServer = NULL;
bool deviceConnected = false;

// Variables to store joystick data
int joyY = 0;
int joyX = 0;

// --- BLE Connection Callbacks ---
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device Connected!");
    }
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device Disconnected!");
      stopMotors(); // Safety: stop if connection is lost
      BLEDevice::startAdvertising(); // Restart advertising
    }
};

// --- BLE Data Receive Callback ---
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue().c_str();
      
      if (rxValue.length() > 0) {
        // Parse the incoming string format: "Y,X\n"
        int commaIndex = rxValue.indexOf(',');
        if (commaIndex > 0) {
          joyY = rxValue.substring(0, commaIndex).toInt();
          joyX = rxValue.substring(commaIndex + 1).toInt();
          
          updateMotors(); // Calculate and apply speeds
        }
      }
    }
};

void setup() {
  Serial.begin(115200);

  // Configure PWM for enable pins (ESP32 Core 3.x API)
  ledcAttach(enableLeftPin, pwmFreq, pwmResolution);
  ledcAttach(enableRightPin, pwmFreq, pwmResolution);

  // Configure direction pins as standard outputs
  pinMode(motorLeftPin1, OUTPUT);
  pinMode(motorLeftPin2, OUTPUT);
  pinMode(motorRightPin1, OUTPUT);
  pinMode(motorRightPin2, OUTPUT);

  stopMotors();

  // Create BLE Device
  BLEDevice::init("ESP32-Chassis"); // Name your web controller looks for
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristic
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                       BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
                     );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service & advertising
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  BLEDevice::startAdvertising();
  Serial.println("BLE Started. Waiting for Web App connection...");
}

void loop() {
  // Everything is handled by BLE callbacks!
  delay(10);
}

// ==========================================
//           MOTOR CONTROL LOGIC
// ==========================================

void updateMotors() {
  // Differential Drive Calculation
  int leftSpeed = joyY + joyX;
  int rightSpeed = joyY - joyX;

  // Constrain speeds to -100 to 100 range
  leftSpeed = constrain(leftSpeed, -100, 100);
  rightSpeed = constrain(rightSpeed, -100, 100);

  // Map to PWM range (0-255)
  int leftPWM = map(abs(leftSpeed), 0, 100, 0, 255);
  int rightPWM = map(abs(rightSpeed), 0, 100, 0, 255);

  // --- Left Motor Direction & Speed ---
  if (leftSpeed > 5) { // Forward
    digitalWrite(motorLeftPin1, HIGH);
    digitalWrite(motorLeftPin2, LOW);
    ledcWrite(enableLeftPin, leftPWM);
  } else if (leftSpeed < -5) { // Backward
    digitalWrite(motorLeftPin1, LOW);
    digitalWrite(motorLeftPin2, HIGH);
    ledcWrite(enableLeftPin, leftPWM);
  } else { // Stop
    digitalWrite(motorLeftPin1, LOW);
    digitalWrite(motorLeftPin2, LOW);
    ledcWrite(enableLeftPin, 0);
  }

  // --- Right Motor Direction & Speed ---
  if (rightSpeed > 5) { // Forward
    digitalWrite(motorRightPin1, HIGH);
    digitalWrite(motorRightPin2, LOW);
    ledcWrite(enableRightPin, rightPWM);
  } else if (rightSpeed < -5) { // Backward
    digitalWrite(motorRightPin1, LOW);
    digitalWrite(motorRightPin2, HIGH);
    ledcWrite(enableRightPin, rightPWM);
  } else { // Stop
    digitalWrite(motorRightPin1, LOW);
    digitalWrite(motorRightPin2, LOW);
    ledcWrite(enableRightPin, 0);
  }
}

void stopMotors() {
  digitalWrite(motorLeftPin1, LOW);
  digitalWrite(motorLeftPin2, LOW);
  digitalWrite(motorRightPin1, LOW);
  digitalWrite(motorRightPin2, LOW);
  ledcWrite(enableLeftPin, 0);
  ledcWrite(enableRightPin, 0);
}