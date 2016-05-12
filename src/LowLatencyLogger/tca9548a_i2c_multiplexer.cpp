#include "arduino.h"
#include <Wire.h>
#include "tca9548a_i2c_multiplexer.h"

Multiplexer tca9548a(TCAADDR);

Multiplexer::Multiplexer(int addr) : addr(addr)
{}

void Multiplexer::switchI2C(int chan)
{
	Wire.beginTransmission(addr);
	Wire.write(1 << chan);
	Wire.endTransmission();
}
