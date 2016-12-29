// bluetooth.h

#ifndef _BLUETOOTH_h
#define _BLUETOOTH_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif

class bluetooth : SoftwareSerial
{
public:
    u8 send_pin, recv_pin;
public:
    bluetooth(u8 _send, u8 _recv);
    void init(u32 baud);
};

#endif

