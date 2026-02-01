#include "imu.h"

#include "packet.h"
extern SensorPacket packet;

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
  // if needed, remap the axis, follow this datasheet page 27 (https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bno055-ds000.pdf)
  // bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_P0);
  // bno.setAxisSign(Adafruit_BNO055::REMAP_SIGN_P0);
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
  quat = bno.getQuat();
  gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  packet.x = (float)quat.x();
  packet.y = (float)quat.y();
  packet.z = (float)quat.z();
  packet.w = (float)quat.w();

  packet.gyroX = (float)gyro.x();  
  packet.gyroY = (float)gyro.y();  
  packet.gyroZ = (float)gyro.z();

  packet.accelX = (float)accel.x(); 
  packet.accelY = (float)accel.y(); 
  packet.accelZ = (float)accel.z();    
}


