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
#include <MPU9250.h>

namespace olive
{

    class mpu9250
    {
        MPU9250 mpu{};          // I2C-Sensor-Lib_iLib 
        f32     measure[6]{};   // buffer for measurements
        
    public:
        // - Note
        //      Initialize external library
        void init() noexcept;

        // - Note
        //      Update measurements in buffer
        void update() noexcept;

        // - Note
        //      Copy of acc-X value 
        f32   ax()      const noexcept;
        // - Note
        //      Copy of acc-Y value 
        f32   ay()      const noexcept;
        // - Note
        //      Copy of acc-Z value 
        f32   az()      const noexcept;

        // - Note
        //      Copy of gyro-X value 
        f32   gx() const noexcept;
        // - Note
        //      Copy of gyro-Y value 
        f32   gy() const noexcept;
        // - Note
        //      Copy of gyro-Z value 
        f32   gz() const noexcept;

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
        i16 acc[3]{};
        i16 gyro[3]{};

        mpu.getMotion6(acc + 0, acc + 1, acc + 2,
                       gyro + 0, gyro + 1, gyro + 2);

        measure[0] = f32(acc[0]) / f32(16384.0);
        measure[1] = f32(acc[1]) / f32(16384.0);
        measure[2] = f32(acc[2]) / f32(16384.0);
        measure[3] = f32(gyro[3]) * 250 / f32(32768.0);
        measure[4] = f32(gyro[4]) * 250 / f32(32768.0);
        measure[5] = f32(gyro[5]) * 250 / f32(32768.0);
    }

    inline f32 mpu9250::ax() const noexcept
    {
        return measure[0];
    }

    inline f32 mpu9250::ay() const noexcept
    {
        return measure[1];
    }

    inline f32 mpu9250::az() const noexcept
    {
        return measure[2];
    }

    inline f32 mpu9250::gx() const noexcept
    {
        return measure[3];
    }

    inline f32 mpu9250::gy() const noexcept
    {
        return measure[4];
    }

    inline f32 mpu9250::gz() const noexcept
    {
        return measure[5];
    }


}

#endif

