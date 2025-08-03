#ifndef _IMU_H
#define _IMU_H


#include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BNO055.h>
// #include <utility/imumaths.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <EEPROM.h>

#define PIN_BUZZER PB_3



class IMUControl {


private:

  // /* members */
  // Adafruit_BNO055 bno = Adafruit_BNO055(55);
  // imu::Vector<3> euler;
  // imu::Vector<3> gyro;
  // imu::Vector<3> accel;


public:

  /* constructor and destructor */
  IMUControl();
  ~IMUControl();

  /* methods */
  // void begin(Adafruit_SSD1306* display);
  void imuService();
};


#endif