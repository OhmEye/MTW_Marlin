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

byte MTWLED_lastpattern;

void MTWLEDSetup()
{
  MTWLED_lastpattern=mtwled_nochange;
  Wire.begin();
  MTWLEDUpdate(mtwled_startup);
}

void MTWLEDUpdate(byte pattern)
{
  if(pattern < 1) return;
  if(pattern != MTWLED_lastpattern)
  {
    Wire.beginTransmission(21);
    Wire.write(pattern);
    Wire.endTransmission();
    MTWLED_lastpattern=pattern;
  }
}

void MTWLEDLogic() // called from main loop
{
  byte pattern = MTWLED_lastpattern;

// maybe replace this with a case tree for all patterns in precedence

  if((degTargetHotend(0) == 0)) {
    if((degHotend(0) > 38)) // heater is off but still hot
      pattern=mtwled_heateroff;
    else
      pattern=mtwled_ready;
  } else {
    if(abs(degTargetHotend(0) - degHotend(0)) < 2) {
      if(isHeatingHotend(0)) pattern=mtwled_templow;
      if(isCoolingHotend(0)) pattern=mtwled_temphigh;
      if(degTargetHotend(0) == degHotend(0)) pattern=mtwled_temphit;  
    }
  }
  MTWLEDUpdate(pattern);
}

void MTWLEDTemp() // called from inside heater function while heater is on
{
	byte pattern;

	if(abs(degTargetHotend(0) - degHotend(0)) > 2) {
	  pattern = 90 + ((degHotend(0) / degTargetHotend(0)) * 10);
	  if(pattern > 99) pattern = 99;
	  MTWLEDUpdate(pattern);
	}
}

#endif //MTWLED

