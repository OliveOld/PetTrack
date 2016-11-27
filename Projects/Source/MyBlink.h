#ifndef _MY_BLINK_H_
#define _MY_BLINK_H_
//#include <CoreLib.hpp>
    class MyBlink
    {
        int led = 0;
    public:
        explicit MyBlink(int _led) noexcept;

        void on() noexcept;
        void off() noexcept;
    };

#endif // _MY_BLINK_H_

