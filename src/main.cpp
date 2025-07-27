#include <Arduino.h>

struct LidarPoint {
  float angle;     // dalam derajat
  float distance;  // dalam mm
  uint8_t quality;
};

bool readLidarPoint(HardwareSerial &serial, LidarPoint &pt) {
  if (serial.available() >= 5) {
    uint8_t buffer[5];
    for (int i = 0; i < 5; i++) {
      buffer[i] = serial.read();
    }

    // Check bit pertama harus 1, bit kedua 0 (start flag dan inversenya)
    bool start_flag = buffer[0] & 0x01;
    bool not_start_flag = buffer[0] & 0x02;
    if (start_flag && !not_start_flag) {
      pt.quality = buffer[0] >> 2;

      uint16_t angle_q6 = ((buffer[2] << 8) | buffer[1]) >> 1;
      pt.angle = angle_q6 / 64.0;  // Q6

      uint16_t dist_q2 = (buffer[4] << 8) | buffer[3];
      pt.distance = dist_q2 / 4.0; // Q2

      return true;
    }
  }
  return false;
}

void startScan(HardwareSerial &serial) {
  serial.write(0xA5);  // SYNC
  serial.write(0x20);  // Start Scan
  delay(10);  
}

void stopScan(HardwareSerial &serial) {
  serial.write(0xA5);  // SYNC
  serial.write(0x25);  // Stop Scan
  delay(10);
}

void resetLidar(HardwareSerial &serial) {
  serial.write(0xA5);  // SYNC
  serial.write(0x40);  // Reset
  delay(10);
}

void getDeviceInfo(HardwareSerial &serial) {
  serial.write(0xA5);  // SYNC
  serial.write(0x50);  // Get Info
  delay(10);
}

void getHealth(HardwareSerial &serial) {
  serial.write(0xA5);  // SYNC
  serial.write(0x52);  // Get Health
  delay(10);
}




void setup() {
  Serial.begin(115200); // Initialize Serial Monitor
  Serial1.begin(460800); // Initialize LiDAR UART communication
  delay(1000); // Wait for the motor to stabilize

  // Kirim perintah Start Scan
  startScan(Serial1); 
}

void loop() {
  LidarPoint pt;
  if (readLidarPoint(Serial1, pt)) {
    Serial.print("📍 Jarak: ");
    Serial.print(pt.distance, 1);
    Serial.print(" mm | Sudut: ");
    Serial.print(pt.angle, 1);
    Serial.print("° | Kualitas: ");
    Serial.println(pt.quality);
  }
}