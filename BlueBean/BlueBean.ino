/*
 * Firmata is a generic protocol for communicating with microcontrollers
 * from software on a host computer. It is intended to work with
 * any host computer software package.
 *
 * To download a host software package, please clink on the following link
 * to open the download page in your default browser.
 *
 * http://firmata.org/wiki/Download
 */

/*
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2011 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  formatted using the GNU C formatting and indenting
*/

/*
 * TODO: use Program Control to load stored profiles from EEPROM
 */

#include <Servo.h>
#include <Wire.h>
#include <Firmata.h>

#include <Arduino.h>

#include <stdint.h>
#include <stdlib.h>

#include "./led.h"
#include "./stop_watch.h"

 // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

led led13{ LED_BUILTIN };
stop_watch sw{};

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
SIGNAL(TIMER0_COMPA_vect) {

}

void setup()
{
    Serial.begin(9600);
    led13.init();
    sw.reset();
}

void loop()
{
    Serial.println(sw.pick());
    led13.change();
    delay(1000);
}

