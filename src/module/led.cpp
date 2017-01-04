#include "./led.h"

led::led(u8 _pin) noexcept
{
    this->pin = _pin;
    pinMode(pin, OUTPUT);   // connect
}

void led::init() noexcept
{
    is_on = false;
    digitalWrite(pin, LOW); // Off
}

void led::change() noexcept
{
    return (is_on == true) ? off() : on();
}

void led::on() noexcept
{
    digitalWrite(pin, HIGH);
    is_on = true;
}

void led::off() noexcept
{
    digitalWrite(pin, LOW);
    is_on = false;
}

