#include <Adafruit_LSM303.h>

class Adafruit_LSM303_ext : public Adafruit_LSM303
{
public:
	void readAccel(int &X, int &Y, int &Z);
	void readMag(int &X, int &Y, int &Z);
};

void LSM303Accel_AcquireData(WriteBuffer *);
void LSM303Mag_AcquireData(WriteBuffer *);