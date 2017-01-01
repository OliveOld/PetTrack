// stop_watch.h

#ifndef _STOP_WATCH_h
#define _STOP_WATCH_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

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
        
        // overflow
        if (curr < start) { 
            return (max - start) + curr;
        }
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

#endif

