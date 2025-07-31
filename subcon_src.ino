#include "button.h"
#include "imu.h"

ButtonActionControl* button = new ButtonActionControl();
IMUControl* imu_controller  = new IMUControl();

struct SensorPacket {
  float roll;    // x-axis orientation
  float pitch;   // y-axis orientation
  float yaw;     // z-axis orientation
  float gyroX;
  float gyroY;
  float gyroZ;
  float accelX;
  float accelY;
  float accelZ;
  float buttonState;
};
SensorPacket packet;

void setup(){
  /* start serial */
  Serial.begin(115200);

  /* set I2C interface */
  Wire.setSDA(PB9);
  Wire.setSCL(PB8);
  Wire.begin();  
  
  /* initialize button and imu */
  button -> begin(imu_controller);
}

void loop(){
  /* repeat buttonService and imuService */
  Serial.write(0x69);
  imu_controller->imuService();
  button -> buttonService();
  
  Serial.write((uint8_t*)&packet, sizeof(packet));
  Serial.write(0x96);
}
