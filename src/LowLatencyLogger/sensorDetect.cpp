#include "arduino.h"
#include <Wire.h>
extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
#include <SparkFun_MMA8452Q.h>
#include "myAssert.h"

MMA8452Q  accel1(0x1D), accel2(0x1C);	// i2c adress as constructor

#define TCAADDR 0x70
#define RTCADDR 0x68

class Multiplexer
{
	int addr;
public:
	Multiplexer(int addr);
	void switchI2C(int chan);
} tca9548a(TCAADDR);

Multiplexer::Multiplexer(int addr) : addr(addr)
{}

void Multiplexer::switchI2C(int chan)
{
	Wire.beginTransmission(addr);
	Wire.write(1 << chan);
	Wire.endTransmission();
}

enum SENSORS { NONE, MMA8452Q_0x1c, MMA8452Q_0x1d} sensors[8];

#define DEBUG_SENSORDETECT

void sensorDetect()	// this is adafruit code from https://learn.adafruit.com/adafruit-tca9548a-1-to-8-i2c-multiplexer-breakout/wiring-and-test
{
	while (!Serial);
	delay(1000);

	Wire.begin();

	Serial.println("\nTCAScanner ready!");

	for (uint8_t t = 0; t<8; t++) {
		tca9548a.switchI2C(t);
		Serial.print("TCA Port #"); Serial.println(t);

		for (uint8_t addr = 0; addr <= 127; addr++) {
			if (addr == TCAADDR) continue;
			if (addr == RTCADDR) continue;

			uint8_t data;
			if (!twi_writeTo(addr, &data, 0, 1, 1)) {
				Serial.print("Found I2C 0x");  Serial.println(addr, HEX);
			}
		}
	}
	Serial.println("\ndone");
}