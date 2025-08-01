#include "imu.h"

#include "packet.h"

IMUControl::IMUControl() {}
IMUControl::~IMUControl() {}

void IMUControl::begin(Adafruit_SSD1306* display) {
  /* Give the info to the display */
  display->clearDisplay();
  display->setCursor(0, 0);
  display->println(">> Initializing IMU\n   ...");
  display->display();

  /* Initialise the sensor */
  while (!bno.begin()) {
  }
  display->println(">> IMU started!");
  display->display();

  /* Check for calibration data on EEPROM */
  int ee_addr = 0;
  long bno_id;
  bool found_calib = false;
  adafruit_bno055_offsets_t calib_data;
  sensor_t sensor;
  EEPROM.get(ee_addr, bno_id);
  bno.getSensor(&sensor);
  if (bno_id != sensor.sensor_id) {
    display->println(">> No calib. data found");
    display->display();
    delay(500);
  } else {
    display->println(">> Calib. data found");
    ee_addr += sizeof(long);
    EEPROM.get(ee_addr, calib_data);
    display->println(">> Restoring calib.\n   data...");
    bno.setSensorOffsets(calib_data);
    display->println(">> IMU calibrated!");
    display->display();

    /* play the buzzer if the IMU successfully calibrated */
    for (uint8_t i = 0; i < 3; ++i) {
      digitalWrite(PIN_BUZZER, HIGH);
      delay(200);
      digitalWrite(PIN_BUZZER, LOW);
      delay(200);
    }
    found_calib = true;
  }
  bno.setExtCrystalUse(true);

  /* Recalibrate IMU if calibration data not found */
  if (!found_calib) {
    sensors_event_t event;
    uint8_t system_cal, gyro_cal, accel_cal, mag_cal;
    while (!bno.isFullyCalibrated()) {
      display->clearDisplay();
      display->setCursor(0, 0);
      display->println(">> Please calibrate:");
      bno.getEvent(&event);

      /* Optional: Display calibration status */
      bno.getCalibration(&system_cal, &gyro_cal, &accel_cal, &mag_cal);
      display->print("S: ");
      display->println(system_cal, DEC);
      display->print("G: ");
      display->println(gyro_cal, DEC);
      display->print("A: ");
      display->println(accel_cal, DEC);
      display->print("M: ");
      display->println(mag_cal, DEC);
      display->display();
    }
    display->clearDisplay();
    display->setCursor(0, 0);
    display->println(">> IMU calibrated!");
    display->display();
    for (uint8_t i = 0; i < 3; ++i) {
      digitalWrite(PIN_BUZZER, HIGH);
      delay(200);
      digitalWrite(PIN_BUZZER, LOW);
      delay(200);
    }

    /* Save the calibration data */
    adafruit_bno055_offsets_t new_calib;
    bno.getSensorOffsets(new_calib);
    display->println(">> Storing calib.\n   data...");
    display->display();
    ee_addr = 0;
    bno.getSensor(&sensor);
    bno_id = sensor.sensor_id;
    EEPROM.put(ee_addr, bno_id);
    ee_addr += sizeof(long);
    EEPROM.put(ee_addr, new_calib);
    display->println(">> Calib. data stored!");
    display->display();
    delay(500);
  }
  display->clearDisplay();
  display->setCursor(0, 28);
  display->println("IMU successfully\ninitiatialized!");
  display->display();
  delay(1000);
}

void IMUControl::imuService() {
  euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  /* Send orientation data
  Serial.print("U");
  Serial.print(euler.x());
  Serial.print("G");
  Serial.print(-euler.y());
  Serial.print("M");
  Serial.print(-euler.z());

  Send gyro data
  Serial.print("D");
  Serial.print(-gyro.x());
  Serial.print("I");
  Serial.print(-gyro.y());
  Serial.print("Y");
  Serial.print(gyro.z());

  Send linear acceleration data
  Serial.print("P");
  Serial.print(-accel.x());
  Serial.print("C");
  Serial.print(accel.y());
  Serial.print("T");
  Serial.print(accel.z()); */

  packet.roll = euler.x();
  packet.pitch = -euler.y();
  packet.yaw = -euler.z();

  packet.gyroX = -gyro.x();
  packet.gyroY = -gyro.y();
  packet.gyroZ = gyro.z();

  packet.accelX = -accel.x();
  packet.accelY = accel.y();
  packet.accelZ = accel.z();
}
