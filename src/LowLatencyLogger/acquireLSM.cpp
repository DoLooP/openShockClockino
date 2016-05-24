#include "acquireData.h"

Adafruit_LSM303_ext lsm;

void Adafruit_LSM303_ext::readAccel(int &X, int &Y, int &Z)
{
	// derived from Adafruit_LSM303::read() reduced to minimum
	// Read the accelerometer
	Wire.beginTransmission((byte)LSM303_ADDRESS_ACCEL);
	Wire.write(LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80);
	Wire.endTransmission();
	Wire.requestFrom((byte)LSM303_ADDRESS_ACCEL, (byte)6);

	// Wait around until enough data is available
	sensorWaitAVG -= micros();
	while (Wire.available() < 6);
	sensorWaitAVG += micros();

	uint8_t xlo = Wire.read();
	uint8_t xhi = Wire.read();
	uint8_t ylo = Wire.read();
	uint8_t yhi = Wire.read();
	uint8_t zlo = Wire.read();
	uint8_t zhi = Wire.read();

	// Shift values to create properly formed integer (low byte first)
	X = (xlo | (xhi << 8)) >> 4;
	Y = (ylo | (yhi << 8)) >> 4;
	Z = (zlo | (zhi << 8)) >> 4;
}

void Adafruit_LSM303_ext::readMag(int &X, int &Y, int &Z)
{
	// derived from Adafruit_LSM303::read() reduced to minimum
	// Read the magnetometer
	Wire.beginTransmission((byte)LSM303_ADDRESS_MAG);
	Wire.write(LSM303_REGISTER_MAG_OUT_X_H_M);
	Wire.endTransmission();
	Wire.requestFrom((byte)LSM303_ADDRESS_MAG, (byte)6);

	// Wait around until enough data is available
	sensorWaitAVG -= micros();
	while (Wire.available() < 6);
	sensorWaitAVG += micros();

	// Note high before low (different than accel)  
	uint8_t xhi = Wire.read();
	uint8_t xlo = Wire.read();
	uint8_t zhi = Wire.read();
	uint8_t zlo = Wire.read();
	uint8_t yhi = Wire.read();
	uint8_t ylo = Wire.read();

	// Shift values to create properly formed integer (low byte first)
	X = (xlo | (xhi << 8));
	Y = (ylo | (yhi << 8));
	Z = (zlo | (zhi << 8));
}

void LSM303Accel_AcquireData(WriteBuffer *wb)
{
	if (acquireLoop == 0)
	{
		assert(lsm.begin());
		wb->print(F("LSMACC_X;LSMACC_Y;LSMACC_Z"));
		return;
	}
	sensorReadAVG -= micros();
	int X, Y, Z;
	lsm.readAccel(X,Y,Z);
	auto tick = micros();
	sensorReadAVG += tick;
	sensorPrintAVG -= tick;
	wb->itoa((short) X);
	wb->write(';');
	wb->itoa((short) Y);
	wb->write(';');
	wb->itoa((short) Z);
	sensorPrintAVG += micros();
}

int magX, magY, magZ;
void LSM303Mag_AcquireData(WriteBuffer *wb)
{
	if (acquireLoop == 0)
	{
		wb->print(F("LSMMAG_X;LSMMAG_Y;LSMMAG_Z"));
		return;
	}
	sensorReadAVG -= micros();
	if (acquireLoop & 1 == 1)	// update only every two lines, because magnetometer rate is 200hz max
		lsm.readMag(magX, magY, magZ);
	auto tick = micros();
	sensorReadAVG += tick;
	sensorPrintAVG -= tick;
	wb->itoa((short) magX);
	wb->write(';');
	wb->itoa((short) magY);
	wb->write(';');
	wb->itoa((short) magZ);
	sensorPrintAVG += micros();
}