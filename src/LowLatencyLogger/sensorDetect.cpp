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

void sensorDetect()	// this is adafruit code from https://learn.adafruit.com/adafruit-tca9548a-1-to-8-i2c-multiplexer-breakout/wiring-and-test
{
	while (!Serial);
	delay(1000);

	Wire.begin();

	Serial.println("\nTCAScanner ready!");
	int sensorIndex = 0;

	for (uint8_t t = 0; t<MAXSENSOR; t++) {
		sensors[sensorIndex].tcaChannel = -1;
		tca9548a.switchI2C(t);
		Serial.print("TCA Port #"); Serial.println(t);

		for (uint8_t addr = 0; addr <= 127; addr++) {
			if (addr == TCAADDR) continue;
			if (addr == RTCADDR) continue;
			uint8_t data;
			if (!twi_writeTo(addr, &data, 0, 1, 1)) {
				Serial.print("Found I2C 0x");  Serial.println(addr, HEX);
				sensors[sensorIndex].tcaChannel = t;
				
				if (addr == 0x1c) sensors[sensorIndex].acquireDataCSV = MM8452Q_0x1c_AcquireDataCSV;
				else if (addr == 0x1d) sensors[sensorIndex].acquireDataCSV = MM8452Q_0x1d_AcquireDataCSV;

				if (sensors[sensorIndex].tcaChannel != -1)
				{
					sensorIndex++;
					if (sensorIndex == MAXSENSOR)
						break;
				}
			}
		}
	}
	if (sensorIndex < MAXSENSOR)
		sensors[sensorIndex].tcaChannel = -1;
	Serial.println("\ndone");
}