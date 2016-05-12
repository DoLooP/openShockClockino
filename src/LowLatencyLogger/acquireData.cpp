#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library

#include "arduino.h"
#include "myAssert.h"
#include "tca9548a_i2c_multiplexer.h"
#include "writeBuffer.h"
#include "acquireData.h"
#include "sensorDetect.h"

MMA8452Q  MM8452Q_0x1c(0x1C), MM8452Q_0x1d(0x1D);	// i2c adress as constructor
int MM8452Q_0x1c_init = false, MM8452Q_0x1d_init = false;

void acquireData(WriteBuffer *wb) {
	int i = 0;
	while (i < 8)
	{
		auto tcaChannel = sensors[i].tcaChannel;
		if ( tcaChannel == -1)
			break;
		tca9548a.switchI2C(tcaChannel);
		if (i)
			wb->write(';');
		sensors[i].acquireDataCSV(wb);
		i++;
	}
	wb->write('\n');
}

void MM8452Q_AcquireDataCSV(MMA8452Q &accel, WriteBuffer *wb)
{
	if (accel.available())
	{
		accel.read();
		wb->print(accel.cx, 3);
		wb->write(';');
		wb->print(accel.cy, 3);
		wb->write(';');
		wb->print(accel.cz, 3);
	}
}

void MM8452Q_0x1c_AcquireDataCSV(WriteBuffer *wb)
{
	if (!MM8452Q_0x1c_init)
	{
		assert(MM8452Q_0x1c.init());
		wb->print("MM8452Q_0x1c X;MM8452Q_0x1c Y;MM8452Q_0x1c Z");
		MM8452Q_0x1c_init = true;
		return;
	}
	MM8452Q_AcquireDataCSV(MM8452Q_0x1c, wb);
}

void MM8452Q_0x1d_AcquireDataCSV(WriteBuffer *wb)
{
	if (!MM8452Q_0x1d_init)
	{
		assert(MM8452Q_0x1d.init());
		wb->print("MM8452Q_0x1d X;MM8452Q_0x1d Y;MM8452Q_0x1d Z");
		MM8452Q_0x1d_init = true;
		return;
	}
	MM8452Q_AcquireDataCSV(MM8452Q_0x1d, wb);
}
