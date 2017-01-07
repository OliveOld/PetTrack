#ifndef _PETS_RAW_SAMPLE_H_
#define _PETS_RAW_SAMPLE_H_
#include "Arduino.h"

#include "./alias.h"
#include "./module/mpu9250.h"

namespace pets
{
    struct SampleData
    {
        f32 ax, ay, az;
        f32 gx, gy, gz;
    };

    Stream& print(Stream& _out, SampleData& buf) 
    {
        static constexpr auto pad = " ";
        _out.print(buf.ax); _out.print(pad);
        _out.print(buf.ay); _out.print(pad);
        _out.print(buf.az); _out.print(pad);
        _out.print(buf.gx); _out.print(pad);
        _out.print(buf.gy); _out.print(pad);
        _out.print(buf.gz);
        return _out;
    }

    SampleData sanpshot(olive::mpu9250& _mpu)
    {
        SampleData smp{};
        smp.ax = _mpu.ax;
        smp.ay = _mpu.ay;
        smp.az = _mpu.az;
        smp.gx = _mpu.gx;
        smp.gy = _mpu.gy;
        smp.gz = _mpu.gz;
        return smp;
    }

}

#endif
