#include "button.h"
#include "imu.h"
#include "packet.h"

ButtonActionControl* button = new ButtonActionControl();
IMUControl* imu_controller  = new IMUControl();
SensorPacket packet;

int data_rate = 100; // Set data rate (Hz)
int period = 1000 / data_rate;

void setup(){
  /* start serial */
  Serial.begin(115200);

  /* set I2C interface */
  // Wire.setSDA(PB9);
  // Wire.setSCL(PB8);
  // Wire.begin();  
  
  /* initialize button and imu */
  // button -> begin(imu_controller);
}

void loop(){
  /* repeat buttonService and imuService */

  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= period) {
    Serial.write(0x69);
    imu_controller->imuService();
    button -> buttonService();
    Serial.write((uint8_t*)&packet, sizeof(packet));
    Serial.write(0x96);

    lastTime = currentTime;
  }
}

