// led.h

#ifndef _LED_h
#define _LED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//#include "../alias.h"

class led
{
    u8 pin;
    bool is_on = false;
public:
    led(u8 _pin) noexcept;
	void init()     noexcept;

    void change()   noexcept;
    void on()       noexcept;
    void off()      noexcept;
};

#endif

