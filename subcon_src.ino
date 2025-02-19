#include "button.h"
#include "imu.h"

ButtonActionControl* button = new ButtonActionControl();
IMUControl* imu_controller  = new IMUControl();

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
  Serial.print("$");
  imu_controller->imuService();
  button -> buttonService();
  Serial.println();
}
