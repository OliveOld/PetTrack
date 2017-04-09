#pragma once
#include <Arduino.h>

class stop_watch
{
    u32 start = millis();
public:
    stop_watch() = default;

    // - Note
    //      Acquire elapsed time in millisecond
    // - See Also
    //      `millis()`
    u32 pick() const noexcept
    {
        static constexpr u32 max = -1;
        const auto curr = millis();

        //// overflow
        //if (curr < start) {
        //}
        return curr - start;    // elapsed
    }

    // - Note
    //      Acquire elapsed time and reset the stop watch
    //      Millisecond
    u32 reset() noexcept
    {
        auto span = pick();
        start = millis();
        return span;
    }

};

