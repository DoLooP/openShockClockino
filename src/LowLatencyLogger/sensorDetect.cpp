#include "arduino.h"
#include <Wire.h>
#include <SparkFun_MMA8452Q.h>
#include "myAssert.h"

MMA8452Q  accel1(0x1D), accel2(0x1C);	// i2c adress as constructor

#define TCAADDR 0x70

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

void sensorDetect()
{
	int i = 8;
	while (i--)
	{
#ifdef DEBUG_SENSORDETECT
		Serial.print("Testing channel ");
		Serial.print(i);
		Serial.println(" ...");
		Serial.println("MMA8452Q_0x1d ?");
#endif
		tca9548a.switchI2C(i);
		if (accel1.init(SCALE_8G, ODR_400))
		{
			Serial.print("Found sensor MMA8452Q_0x1d connected on channel #");
			Serial.println(i);
			sensors[i] = MMA8452Q_0x1d;
		}

#ifdef DEBUG_SENSORDETECT
		Serial.println("MMA8452Q_0x1d ?");
#endif
		if (accel2.init(SCALE_8G, ODR_400))
		{
			Serial.print("Found sensor MMA8452Q_0x1c connected on channel #");
			Serial.println(i);
			sensors[i] = MMA8452Q_0x1c;
		}
	}
	/*
		assert(accel1.init(SCALE_8G, ODR_400));
		Serial.println("MMA8452Q #1 OK.");
		assert(accel2.init(SCALE_8G, ODR_400));
		Serial.println("MMA8452Q #2 OK.");
	*/
}