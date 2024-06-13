#include <Adafruit_Fingerprint.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pin definitions
#define FINGERPRINT_RX_PIN 2
#define FINGERPRINT_TX_PIN 3
#define GPS_RX_PIN 4
#define GPS_TX_PIN 5
#define SERVO_PIN 9

// Fingerprint sensor and GPS setup
SoftwareSerial fingerSerial(FINGERPRINT_RX_PIN, FINGERPRINT_TX_PIN);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus gps;

// Servo setup
Servo myServo;
int lockPosition = 0;
int unlockPosition = 90;
bool isLocked = true;  // Lock state: true = locked, false = unlocked

void setup() {
  Serial.begin(9600);
  fingerSerial.begin(57600);
  gpsSerial.begin(9600);
  myServo.attach(SERVO_PIN);
  myServo.write(lockPosition);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1)
      ;
  }
}

void loop() {
  // Read GPS data
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
    Serial.print("Latitude= ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(" Longitude= ");
    Serial.println(gps.location.lng(), 6);
  }

  // Fingerprint detection
  if (getFingerprintID() >= 0) {
    toggleLock();
    delay(1000);  // Debounce delay
  }

  delay(100);
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}

void toggleLock() {
  if (isLocked) {
    Serial.println("Fingerprint matched, unlocking...");
    myServo.write(unlockPosition);
    isLocked = false;
  } else {
    Serial.println("Fingerprint matched, locking...");
    myServo.write(lockPosition);
    isLocked = true;
  }
}
