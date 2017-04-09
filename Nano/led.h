#pragma once
#include <Arduino.h>

class led
{
    u8 pin;
    bool is_on = false;
public:
    led(u8 _pin) noexcept {
        this->pin = _pin;
        pinMode(pin, OUTPUT);   // connect
    }
    void init()     noexcept {
        is_on = false;
        digitalWrite(pin, LOW); // Off
    }
    void change()   noexcept {
        return (is_on == true) ? off() : on();
    }

    void on()       noexcept
    {
        digitalWrite(pin, HIGH);
        is_on = true;
    }
    void off()      noexcept
    {
        digitalWrite(pin, LOW);
        is_on = false;
    }
};
