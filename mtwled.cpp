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
unsigned long MTWLED_timer;
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
  if(pattern.part[0] < 1) return;
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
  int swing;
  boolean endstophit=false;
  
  if(MTWLED_control==1 || MTWLED_control==255) return;
  
  #if defined(X_MIN_PIN) && X_MIN_PIN > -1
  if(READ(X_MIN_PIN)^X_MIN_ENDSTOP_INVERTING) {
    pattern.value=MTWLEDConvert(mtwled_endstopx);
    endstophit=true;
  }
  #endif
  #if defined(X_MAX_PIN) && X_MAX_PIN > -1
  if(READ(X_MAX_PIN)^X_MAX_ENDSTOP_INVERTING) {
    pattern.value=MTWLEDConvert(mtwled_endstopx);
    endstophit=true;
  }
  #endif
  #if defined(Y_MIN_PIN) && Y_MIN_PIN > -1
  if(READ(Y_MIN_PIN)^Y_MIN_ENDSTOP_INVERTING) {
    pattern.value=MTWLEDConvert(mtwled_endstopy);
    endstophit=true;
  }
  #endif
  #if defined(Y_MAX_PIN) && Y_MAX_PIN > -1
  if(READ(Y_MAX_PIN)^Y_MAX_ENDSTOP_INVERTING) {
    pattern.value=MTWLEDConvert(mtwled_endstopy);
    endstophit=true;
  }
  #endif
  #if defined(Z_MIN_PIN) && Z_MIN_PIN > -1
  if(READ(Z_MIN_PIN)^Z_MIN_ENDSTOP_INVERTING) {
    pattern.value=MTWLEDConvert(mtwled_endstopz);
    
  }
  #endif
  #if defined(Z_MAX_PIN) && Z_MAX_PIN > -1
  if(READ(Z_MAX_PIN)^Z_MAX_ENDSTOP_INVERTING) {
    pattern.value=MTWLEDConvert(mtwled_endstopz);
    endstophit=true;
  }
  #endif
  if(endstophit)
    {
      MTWLEDUpdate(pattern,MTWLED_endstoptimer);
      return;
    }  
  
  if(pattern.value==mtwled_nochange) return;
  if(MTWLED_timer > millis()) return;

  if((degTargetHotend(0) == 0)) {
    if((degHotend(0) > MTWLED_cool)) // heater is off but still warm
      pattern.value=MTWLEDConvert(mtwled_heateroff);
    else
      pattern.value=MTWLEDConvert(mtwled_ready);
    MTWLEDUpdate(pattern);
  } else {
    swing=abs(degTargetHotend(0) - degHotend(0)); // how far off from target temp we are
    if(swing < MTWLED_swing*2) {                  // if within double the swing threshold
      if(isHeatingHotend(0)) pattern.value=MTWLEDConvert(mtwled_templow);    // heater is on so temp must be low
      if(isCoolingHotend(0)) pattern.value=MTWLEDConvert(mtwled_temphigh);   // heater is off so temp must be high
      if(swing < MTWLED_swing) pattern.value=MTWLEDConvert(mtwled_temphit);  // close to target temp, so consider us 'at temp'
      MTWLEDUpdate(pattern);
    } 
  }
}

void MTWLEDTemp() // called from inside heater function while heater is on to to the percentile display
{
	byte pattern;
        if(MTWLED_control==255) return;
	if(abs(degTargetHotend(0) - degHotend(0)) > MTWLED_swing*2) {
	  pattern = 90 + ((degHotend(0) / degTargetHotend(0)) * 10);
	  if(pattern > 99) pattern = 99;
	  MTWLEDUpdate(pattern);
	}
}

#endif //MTWLED

