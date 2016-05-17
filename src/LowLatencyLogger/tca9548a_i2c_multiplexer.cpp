#include "arduino.h"
#include <Wire.h>
#include "tca9548a_i2c_multiplexer.h"

Multiplexer tca9548a(TCAADDR);

Multiplexer::Multiplexer(int addr) : addr(addr)
{}

//#define MULTIPLEXER_DEBUG
void Multiplexer::switchI2C(int chan)
{
	if (lastChannel == chan)
		return;
#ifdef MULTIPLEXER_DEBUG
	Serial.print(F("Switching to lines 0x"));
	Serial.print(chan,HEX);
	Serial.print(F(" ..."));
	Serial.flush();
#endif
	Wire.beginTransmission(addr);
	Wire.write(chan);
	Wire.endTransmission();
	lastChannel = chan;
#ifdef MULTIPLEXER_DEBUG
	Serial.println(F(" DONE"));
#endif
}
