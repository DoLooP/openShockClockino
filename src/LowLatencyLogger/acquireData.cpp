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

//#define DEBUG_ACQUIREDATA
#ifdef DEBUG_ACQUIREDATA
//#define DEBUG_ACQUIREDATA_INIT
#endif 

void acquireData(WriteBuffer *wb) {
	int i = 0;
	while (i < 8)
	{
#ifdef DEBUG_ACQUIREDATA
		Serial.print("Sensor #");
		Serial.print(i);
		Serial.print(" ...");
		Serial.flush();
#endif
		auto tcaChannel = sensors[i].tcaChannel;
		if ( tcaChannel == -1)
			break;
#ifdef DEBUG_ACQUIREDATA
		Serial.print(" SwitchI2c(");
		Serial.print(tcaChannel);
		Serial.print(") ...");
		Serial.flush();
#endif
		tca9548a.switchI2C(tcaChannel);
		if (i)
			wb->write(';');
		sensors[i].acquireDataCSV(wb);
		i++;
	}
	wb->write('\n');
#ifdef DEBUG_ACQUIREDATA
	Serial.println(" DONE");
	Serial.flush();
#endif
}

void MM8452Q_AcquireDataCSV(MMA8452Q &accel, WriteBuffer *wb)
{
#ifdef DEBUG_ACQUIREDATA
	Serial.print("MM8452Q available ...");
	Serial.flush();
#endif
	if (accel.available())
	{
#ifdef DEBUG_ACQUIREDATA
		Serial.print(" read ...");
		Serial.flush();
#endif
		accel.read();
		wb->print(accel.cx);
		wb->write(';');
		wb->print(accel.cy);
		wb->write(';');
		wb->print(accel.cz);
	}
#ifdef DEBUG_ACQUIREDATA
	Serial.println(" DONE");
	Serial.flush();
#endif
}

void MM8452Q_0x1c_AcquireDataCSV(WriteBuffer *wb)
{
	if (!MM8452Q_0x1c_init)
	{
#ifdef DEBUG_ACQUIREDATA_INIT
		Serial.print("MM8452Q_0x1c.init ...");
		Serial.flush();
#endif
		assert(MM8452Q_0x1c.init());
		wb->print("MM8452Q_0x1c X;MM8452Q_0x1c Y;MM8452Q_0x1c Z");
		MM8452Q_0x1c_init = true;
#ifdef DEBUG_ACQUIREDATA_INIT
		Serial.println(" DONE");
		Serial.flush();
#endif
		return;
	}
	MM8452Q_AcquireDataCSV(MM8452Q_0x1c, wb);
}

void MM8452Q_0x1d_AcquireDataCSV(WriteBuffer *wb)
{
	if (!MM8452Q_0x1d_init)
	{
#ifdef DEBUG_ACQUIREDATA_INIT
		Serial.print("MM8452Q_0x1c.init ...");
		Serial.flush();
#endif
		assert(MM8452Q_0x1d.init());
		wb->print("MM8452Q_0x1d X;MM8452Q_0x1d Y;MM8452Q_0x1d Z");
		MM8452Q_0x1d_init = true;
#ifdef DEBUG_ACQUIREDATA_INIT
		Serial.println(" DONE");
		Serial.flush();
#endif
		return;
	}
	MM8452Q_AcquireDataCSV(MM8452Q_0x1d, wb);
}
