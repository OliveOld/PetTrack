// imu_sensor.h

#ifndef _IMU_SENSOR_h
#define _IMU_SENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif
#include "../alias.h"

//#include <i2c_MPU9250.h>
#include "../extlib/MPU9250.h"

namespace olive
{

    class mpu9250
    {
        MPU9250 mpu{};
    public:
        // - Note
        //      Initialize external library
        void init() noexcept;

        // - Note
        //      Update measurements in buffer
        void update() noexcept;

    public:
        // - Note
        //      Copy of acc-X,Y,Z value 
        f32   ax, ay, az;

        // - Note
        //      Copy of gyro-X,Y,Z value 
        f32   gx, gy, gz;

    };

}

namespace olive
{
    inline void mpu9250::init() noexcept
    {
        mpu.initialize();
    }

    inline void mpu9250::update() noexcept
    {
        // for raw data
        i16 r_ax, r_ay, r_az;
        i16 r_gx, r_gy, r_gz;

        mpu.getMotion6(&r_ax, &r_ay, &r_az, &r_gx, &r_gy, &r_gz);

        ax = static_cast<double>(r_ax) / 16384;
        ay = static_cast<double>(r_ay) / 16384;
        az = static_cast<double>(r_az) / 16384;

        gx = static_cast<double>(r_gx) * 250 / 32768;
        gy = static_cast<double>(r_gy) * 250 / 32768;
        gz = static_cast<double>(r_gz) * 250 / 32768;
    }


}

#endif

