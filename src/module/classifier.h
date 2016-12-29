// classifier.h

#ifndef _CLASSIFIER_h
#define _CLASSIFIER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// Posture Transition Class
struct transition
{
public:
    enum class label
    {
        Unknown,
        SiSt,
        StSi,
        LySi,
        SiLy,
        SiWk,
        WkSi
    };

public:
    label lbl = label::Unknown;

public:

};

class classifier
{
public:
    void init();
};

#endif

