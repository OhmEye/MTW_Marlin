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
  // maybe do something in main marlin loop? if target temp goes to zero, assume print is done, etc?
}

void MTWLEDTemp()
{
	byte pattern = 90 + ((degHotend(0) / degTargetHotend(0)) * 10);
	if(pattern > 99) pattern=99;
	MTWLEDUpdate(pattern);
}

#endif //MTWLED

