# The Firmware for STM32 at Altair Team

## IMU
The `imu.cpp` file contains the implementation of the `IMUControl` class, which is responsible for initializing and servicing the Inertial Measurement Unit (IMU) sensor. Below is a brief explanation of the key functions in this file:

The data is sent in the following format:
- Orientation data: `U` for x-axis, `G` for y-axis (negated), `M` for z-axis (negated). Orientation data is in the form of euler angle which is the roll, pitch, yaw relative to the fixed frame.
- Gyroscope data: `D` for x-axis (negated), `I` for y-axis (negated), `Y` for z-axis. Gyroscope data is in the form of velocity angle, rad/s
- Linear acceleration data: `P` for x-axis (negated), `C` for y-axis, `T` for z-axis. 

## Button

The `button.cpp` file contains the implementation of the `ButtonControl` class, which is responsible for handling button presses and executing corresponding actions. Below is a brief explanation of the key functions in this file:

Key Features:
- Debounce the button press.
- Execute the corresponding action based on the button state.

The data is sent in the following format:
- Button state: `B` for the state

## Serial Data
The complete data is sent to the computer via USB Serial with this following format:
"$U{}G{}M{}D{}I{}Y{}P{}C{}T{}B{}\n"

- $ is the initial marker 
- \n is the end marker 
- {} is the value placeholder 


