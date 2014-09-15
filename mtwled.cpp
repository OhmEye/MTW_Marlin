/*
  mtwled.cpp
  Makers Tool Works RGB LED I2C control
*/

#include "Marlin.h"
#ifdef MTWLED

#if (ARDUINO >= 100)
  # include "Arduino.h"
#else
  # include "WProgram.h"
#endif

#include "mtwled.h"
#include "temperature.h"

void MTWLEDUpdate(byte pattern)
{
  Wire.begin(); 
  Wire.beginTransmission(74);
  Wire.write(pattern);
  Wire.endTransmission();
}

void MTWLEDLogic()
{
  extern byte lastpattern;
  byte pattern = lastpattern;

  // maybe do something in main marlin loop? if target temp goes to zero, assume print is done, etc?

  if(degTargetHotend(0) == 0) { // maybe replace this with a case tree for all patterns in precedence
	pattern = mtwled_ready;
  } else if(abs(degTargetHotend(0) - degHotend(0)) < 5) {
    if(isHeatingHotend(0)) pattern=mtwled_heateron;
    if(isCoolingHotend(0)) pattern=mtwled_heateroff;
  }

  // update if pattern changed
  if(pattern != lastpattern) {
	MTWLEDUpdate(pattern);
	lastpattern = pattern;
  }
}

void MTWLEDTemp()
{
	byte pattern;

	if(abs(degTargetHotend(0) - degHotend(0)) > 5) {
	  pattern = 90 + ((degHotend(0) / degTargetHotend(0)) * 10);
	  if(pattern > 99) pattern = 99;
	  MTWLEDUpdate(pattern);
	}
}

#endif //MTWLED

