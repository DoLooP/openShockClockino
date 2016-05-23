#include "acquireData.h"

Adafruit_LSM303 lsm;

void LSM303Accel_AcquireData(WriteBuffer *wb)
{
	if (acquireLoop == 0)
	{
		assert(lsm.begin());
		wb->print(F("LSMACC_X;LSMACC_Y;LSMACC_Z"));
		return;
	}
	lsm.read();
	wb->itoa((short)lsm.accelData.x);
	wb->write(';');
	wb->itoa((short)lsm.accelData.y);
	wb->write(';');
	wb->itoa((short)lsm.accelData.z);
}

void LSM303Mag_AcquireData(WriteBuffer *wb)
{
	if (acquireLoop == 0)
	{
		wb->print(F("LSMMAG_X;LSMMAG_Y;LSMMAG_Z"));
		return;
	}
	wb->itoa((short)lsm.magData.x);
	wb->write(';');
	wb->itoa((short)lsm.magData.y);
	wb->write(';');
	wb->itoa((short)lsm.magData.z);
}