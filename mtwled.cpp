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
long MTWLED_timer;
int MTWLED_control;

void MTWLEDSetup()
{
  MTWLED_lastpattern=mtwled_nochange;
  MTWLED_timer=0;
  MTWLED_control=0;
  Wire.begin();
  MTWLEDUpdate(mtwled_startup);
}

void MTWLEDUpdate(byte pattern, byte red, byte green, byte blue, unsigned long timer, int control)
{
  if(control>=0) MTWLED_control=control;
  if(pattern < 1 || MTWLED_control==255) return;
  if(pattern != MTWLED_lastpattern)
  {    
    Wire.beginTransmission(21);
    Wire.write(pattern);
    Wire.endTransmission();
    MTWLED_lastpattern=pattern;
    if(timer) MTWLED_timer=millis()+(timer*1000);
    SERIAL_PROTOCOL("MTWLED ");
    SERIAL_PROTOCOLLN((int)pattern);
  }
}

void MTWLEDLogic() // called from main loop
{
  byte pattern = MTWLED_lastpattern;
  
  if(MTWLED_control==1) return;
  if(pattern==mtwled_nochange) return;
  if(MTWLED_timer > millis()) return;

// maybe replace this with a case tree for all patterns in precedence

  if((degTargetHotend(0) == 0)) {
    if((degHotend(0) > 38)) // heater is off but still hot
      pattern=mtwled_heateroff;
    else
      pattern=mtwled_ready;
    MTWLEDUpdate(pattern);
  } else {
    if(abs(degTargetHotend(0) - degHotend(0)) < 2) {
      if(isHeatingHotend(0)) pattern=mtwled_templow;
      if(isCoolingHotend(0)) pattern=mtwled_temphigh;
      if(degTargetHotend(0) == degHotend(0)) pattern=mtwled_temphit;
      MTWLEDUpdate(pattern);
    }
  }
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

