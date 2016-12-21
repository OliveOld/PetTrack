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
public:
    void init();

    int pick();
    int reset();
};

#endif

