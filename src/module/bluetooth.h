// bluetooth.h

#ifndef _BLUETOOTH_h
#define _BLUETOOTH_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif
#include <SoftwareSerial.h>

namespace olive
{
    class bluetooth final: 
            public SoftwareSerial
    {
    public:
        bluetooth(u8 _send, u8 _recv);
        void init(u32 baud);
    };

}

namespace olive
{
    bluetooth::bluetooth(u8 _send, u8 _recv) :
        SoftwareSerial(_send, _recv)
    {

    }

    void bluetooth::init(u32 baud)
    {
        this->begin(baud);
    }


}

#endif

