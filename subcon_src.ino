#include "button.h"
#include "imu.h"

ButtonActionControl* button = new ButtonActionControl();
IMUControl* imu_controller  = new IMUControl();

// CONFIGURATION (Change this based on motion program rate/hz, ask motion programmer)
int data_rate = 100; // Set data rate
int period = 1000 / data_rate;

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
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime >= period) {
    /* repeat buttonService and imuService */
    Serial.print("$");
    imu_controller->imuService();
    button->buttonService();
    Serial.println();

    lastTime = currentTime;
  }
}
