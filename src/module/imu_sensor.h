// imu_sensor.h

#ifndef _IMU_SENSOR_h
#define _IMU_SENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif

//#include "I2C-Sensor-Lib_iLib\src\i2c_MPU9250.h"

class imu_sensor
{

 public:
    void init();
};

#endif

