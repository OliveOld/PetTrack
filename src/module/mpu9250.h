// imu_sensor.h

#ifndef _IMU_SENSOR_h
#define _IMU_SENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif
#include "../alias.h"

#include <i2c_MPU9250.h>

namespace olive
{

    class mpu9250
    {
        MPU9250 mpu{};          // I2C-Sensor-Lib_iLib 
        f32     measure[7]{};   // buffer for measurements
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


        f32   temp() const noexcept;

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
        mpu.getMeasurement(measure);
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

    inline f32 mpu9250::temp() const noexcept
    {
        return measure[3];
    }

    inline f32 mpu9250::gx() const noexcept
    {
        return measure[4];
    }

    inline f32 mpu9250::gy() const noexcept
    {
        return measure[5];
    }

    inline f32 mpu9250::gz() const noexcept
    {
        return measure[6];
    }


}

#endif

