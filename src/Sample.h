#ifndef _PETS_RAW_SAMPLE_H_
#define _PETS_RAW_SAMPLE_H_
#include "Arduino.h"

#include "./alias.h"

namespace pets
{

    struct Sample
    {
        f32 data[6]{};

    public:
        Sample() noexcept = default;
        Sample(const Sample& _rhs) noexcept
        {
            memcpy(this->data, _rhs.data, sizeof(f32) * 6);
        };
        Sample& operator=(const Sample& _rhs) noexcept
        {
            memcpy(this->data, _rhs.data, sizeof(f32) * 6);
            return *this;
        }
    };

    Stream& operator << (Stream& _out, const pets::Sample& _smp) noexcept
    {
        _out.print(_smp.data[0]);
        _out.print(", ");
        _out.print(_smp.data[1]);
        _out.print(", ");
        _out.print(_smp.data[2]);
        _out.print(", ");
        _out.print(_smp.data[3]);
        _out.print(", ");
        _out.print(_smp.data[4]);
        _out.print(", ");
        _out.print(_smp.data[5]);
        return _out;
    }
}

#endif
