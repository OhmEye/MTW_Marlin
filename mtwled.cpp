/*
  mtwled.cpp
  Makers Tool Works RGB LED I2C control
  Contributed by OhmEye October 2014
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

patterncode MTWLED_lastpattern;
long MTWLED_timer;
int MTWLED_control;
boolean MTWLED_feedback=false;

void MTWLEDSetup()
{
  MTWLED_lastpattern.value=0;
  MTWLED_timer=0;
  MTWLED_control=0;
  Wire.begin();
  MTWLEDUpdate(MTWLEDConvert(mtwled_startup));
}

uint32_t MTWLEDConvert(byte pattern, byte red, byte green, byte blue)
{
  patterncode pc;
  pc.part[0]=pattern;
  pc.part[1]=red;
  pc.part[2]=green;
  pc.part[3]=blue;
  return pc.value;
}


void MTWLEDUpdate(byte pattern, byte red, byte green, byte blue, unsigned long timer, int control)
{
  patterncode pc;
  pc.part[0]=pattern;
  pc.part[1]=red;
  pc.part[2]=green;
  pc.part[3]=blue;
  MTWLEDUpdate(pc,timer,control);  
}

void MTWLEDUpdate(patterncode pattern, unsigned long timer, int control) // send pattern frame via I2C
{
  byte sout[]={250,pattern.part[0],pattern.part[1],pattern.part[2],pattern.part[3]}; // build frame
  
  if(control>=0) MTWLED_control=control;                  // handle exceptions/collisions/control
  if(control==254) MTWLED_feedback=!MTWLED_feedback;
  if(pattern.part[0] < 1 || MTWLED_control==255) return;
  if(pattern.value != MTWLED_lastpattern.value)           // don't sent sequential identical patterns
  {    
    Wire.beginTransmission(21);
     Wire.write(sout,5);                                  // send the 5 byte frame
    Wire.endTransmission();
    MTWLED_lastpattern=pattern;                           // update states
    if(timer) MTWLED_timer=millis()+(timer*1000);
  if(MTWLED_feedback)
    {
    SERIAL_PROTOCOL("LED P:");                           // print feedback to serial
    SERIAL_PROTOCOL((int)pattern.part[0]);
    SERIAL_PROTOCOL(" R:");
    SERIAL_PROTOCOL((int)pattern.part[1]);
    SERIAL_PROTOCOL(" E:");
    SERIAL_PROTOCOL((int)pattern.part[2]);
    SERIAL_PROTOCOL(" B:");
    SERIAL_PROTOCOLLN((int)pattern.part[3]);
//    SERIAL_PROTOCOL("MTWLED ");
//    SERIAL_PROTOCOLLN((uint32_t)pattern.value);
    }
  }
}

void MTWLEDLogic() // called from main loop
{
  patterncode pattern = MTWLED_lastpattern;
  
  if(MTWLED_control==1) return;
  if(pattern.value==mtwled_nochange) return;
  if(MTWLED_timer > millis()) return;

  if((degTargetHotend(0) == 0)) {
    if((degHotend(0) > 38)) // heater is off but still warm
      pattern.value=MTWLEDConvert(mtwled_heateroff);
    else
      pattern.value=MTWLEDConvert(mtwled_ready);
    MTWLEDUpdate(pattern);
  } else {
    if(abs(degTargetHotend(0) - degHotend(0)) < 2) {
      if(isHeatingHotend(0)) pattern.value=MTWLEDConvert(mtwled_templow);
      if(isCoolingHotend(0)) pattern.value=MTWLEDConvert(mtwled_temphigh);
      if(degTargetHotend(0) == degHotend(0)) pattern.value=MTWLEDConvert(mtwled_temphit);
      MTWLEDUpdate(pattern);
    }
  }
}

void MTWLEDTemp() // called from inside heater function while heater is on to to the percentile display
{
	byte pattern;

	if(abs(degTargetHotend(0) - degHotend(0)) > 2) {
	  pattern = 90 + ((degHotend(0) / degTargetHotend(0)) * 10);
	  if(pattern > 99) pattern = 99;
	  MTWLEDUpdate(pattern);
	}
}

#endif //MTWLED

