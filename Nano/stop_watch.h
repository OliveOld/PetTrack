#pragma once
#include <Arduino.h>

using duration = u32;

class stop_watch
{
    duration start = millis();
public:
    stop_watch() = default;

    // - Note
    //      Acquire elapsed time in millisecond
    // - See Also
    //      `millis()`
    duration pick() const noexcept
    {
        //static constexpr u32 max = -1;
        const duration curr = millis();

        //// overflow
        //if (curr < start) {
        //}
        return curr - start;    // elapsed
    }

    // - Note
    //      Acquire elapsed time and reset the stop watch
    //      Millisecond
    duration reset() noexcept
    {
        duration span = pick();
        start = millis();
        return span;
    }

};

