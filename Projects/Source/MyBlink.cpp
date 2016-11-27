
//#if defined(ARDUINO) && ARDUINO >= 100
//#else
//#include "WProgram.h"
//#endif
#include <Arduino.h>
#include "./MyBlink.h"

MyBlink::MyBlink(int _led) noexcept :
    led{ _led }
{
    pinMode(this->led, OUTPUT);      // sets the digital pin as output
}

void MyBlink::on() noexcept {
    
    digitalWrite(this->led, HIGH);   // sets the LED on
}

void MyBlink::off() noexcept {
    digitalWrite(this->led, LOW);   // sets the LED on
}

