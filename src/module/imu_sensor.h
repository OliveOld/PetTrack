// imu_sensor.h

#ifndef _IMU_SENSOR_h
#define _IMU_SENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif

class imu_sensor
{

 public:
    void init();
};

#endif

