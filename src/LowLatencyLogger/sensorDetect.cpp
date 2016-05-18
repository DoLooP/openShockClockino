#include "arduino.h"
#include <Wire.h>
extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
#include "myAssert.h"
#include "tca9548a_i2c_multiplexer.h"
#include "sensorDetect.h"
#include "acquireData.h"

SensorDef sensors[MAXSENSOR];

#define RTCADDR 0x68

int sensorConflict(SensorDef *sensor)
{
	for (uint8_t t = 0; t < MAXSENSOR && sensors[t].tcaChannel != TCA_INVALID; t++)
	{
		if (sensors[t].i2cAddr == sensor->i2cAddr && (sensors + t) != sensor)
		{
			Serial.print(F("Sensor "));
			Serial.print(sensor->i2cAddr, HEX);
			Serial.print(F(" conflicts with channel "));
			Serial.println(sensors[t].tcaChannel);
			return 1;
		}
	}
	return 0;
}

void sensorOptimizeI2C()
{
	int tcaChannelGroup = 0;
	for (uint8_t t = 0; t < MAXSENSOR && sensors[t].tcaChannel != TCA_INVALID; t++) {
		if (!sensorConflict(sensors + t))
		{
			Serial.print(F("Sensor "));
			Serial.print(sensors[t].i2cAddr, HEX);
			Serial.print(F(" added to I2c group channel: 0x"));
			tcaChannelGroup |= sensors[t].tcaChannel;
			Serial.println(tcaChannelGroup,HEX);
		}
	}
	for (uint8_t t = 0; t < MAXSENSOR && sensors[t].tcaChannel != TCA_INVALID; t++) {
		if ((sensors[t].tcaChannel & tcaChannelGroup) == sensors[t].tcaChannel)
			sensors[t].tcaChannel = tcaChannelGroup;
	}
}

void sensorDetect()	// this is adafruit code from https://learn.adafruit.com/adafruit-tca9548a-1-to-8-i2c-multiplexer-breakout/wiring-and-test
{
	while (!Serial);
	delay(10);

	Wire.begin();

	Serial.println(F("\nTCAScanner ready!"));
	int sensorIndex = 0;

	for (uint8_t t = 0; t<MAXSENSOR; t++) {
		sensors[sensorIndex].tcaChannel = TCA_INVALID;
		tca9548a.switchI2C(1 << t);
		Serial.print(F("TCA Port #")); Serial.println(t);

		for (uint8_t addr = 0; addr <= 127; addr++) {
			if (addr == TCAADDR) continue;
			if (addr == RTCADDR) continue;
			uint8_t data;
			if (!twi_writeTo(addr, &data, 0, 1, 1)) {
				Serial.print(F("Found I2C 0x"));  Serial.println(addr, HEX);
				sensors[sensorIndex].tcaChannel = (1 << t);
				sensors[sensorIndex].i2cAddr = addr;

				// choose right driver
				if (addr == 0x1c) sensors[sensorIndex].acquireDataCSV = MM8452Q_0x1c_AcquireDataCSV;
				else if (addr == 0x1d) sensors[sensorIndex].acquireDataCSV = MM8452Q_0x1d_AcquireDataCSV;
				else sensors[sensorIndex].tcaChannel = TCA_INVALID;

				if (sensors[sensorIndex].tcaChannel != TCA_INVALID) {
					sensorIndex++;
					if (sensorIndex == MAXSENSOR)
						break;
				}
			}
		}
	}
	if (sensorIndex < MAXSENSOR)
		sensors[sensorIndex].tcaChannel = TCA_INVALID;
	Serial.println(F("\nDONE"));
	sensorOptimizeI2C();
}