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

  // /* set I2C interface */
  // Wire.setSDA(PB9);
  // Wire.setSCL(PB8);
  // Wire.begin();  
  
  // /* initialize button and imu */
  // button -> begin(imu_controller);
}

void loop(){
  /* repeat buttonService and imuService */

  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= period) {
    
    imu_controller->imuService();
    button -> buttonService();
    
    uint8_t checksum = 0;
    uint8_t* data = (uint8_t*)&packet;

    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(sizeof(SensorPacket) + 1);
    for (size_t i = 0; i < sizeof(SensorPacket); i++) {
      Serial.write(data[i]);
      checksum += data[i];
    }
    checksum = ~checksum;
    Serial.write(checksum);

    lastTime = currentTime;
  }
}

