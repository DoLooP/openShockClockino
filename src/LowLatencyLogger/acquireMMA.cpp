#include "acquireData.h"

MMA8452Q  MM8452Q_0x1c(0x1C), MM8452Q_0x1d(0x1D);	// i2c adress as constructor

void MM8452Q_AcquireDataCSV(MMA8452Q &accel, WriteBuffer *wb)
{
	if (acquireLoop == 0)
	{
#ifdef DEBUG_MMA
		Serial.print(F("MM8452Q_0x1c.init ..."));
		Serial.flush();
#endif
		assert(accel.init(SCALE_8G, ODR_400));
		wb->print(F("MM8452Q_X;MM8452Q_Y;MM8452Q_Z"));
#ifdef DEBUG_MMA
		Serial.println(F(" DONE");
		Serial.flush();
#endif
		wireInit();
		return;
	}

#ifdef DEBUG_MMA
	Serial.print(F("MM8452Q available ..."));
	Serial.flush();
#endif
	/*
	sensorWaitAVG  -= micros();
	while (!accel.available());	// sync should come from mainSync
	sensorWaitAVG += micros();
	*/
#ifdef DEBUG_MMA
	Serial.print(F(" read ..."));
	Serial.flush();
#endif
	sensorReadAVG -= micros();
	accel.read();
	sensorReadAVG += micros();
	sensorPrintAVG -= micros();
	wb->itoa(accel.x);
	wb->write(';');
	wb->itoa(accel.y);
	wb->write(';');
	wb->itoa(accel.z);
	sensorPrintAVG += micros();
#ifdef DEBUG_MMA
	Serial.println(F(" DONE"));
	Serial.flush();
#endif
}

void MM8452Q_0x1c_AcquireDataCSV(WriteBuffer *wb)
{
	MM8452Q_AcquireDataCSV(MM8452Q_0x1c, wb);
}

void MM8452Q_0x1d_AcquireDataCSV(WriteBuffer *wb)
{
	MM8452Q_AcquireDataCSV(MM8452Q_0x1d, wb);
}


