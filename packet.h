#pragma pack(push, 1)
struct SensorPacket {
  float x;
  float y;
  float z;
  float w;
  float gyroX;
  float gyroY;
  float gyroZ;
  float accelX;
  float accelY;
  float accelZ;
  float buttonState;
};
#pragma pack(pop)
