void sensorDetect();

class WriteBuffer;
typedef void (*SensorAcquireDataCSV)(WriteBuffer *);

struct SensorDef {
	int tcaChannel;
	unsigned char i2cAddr;
	SensorAcquireDataCSV acquireDataCSV;
};

#define MAXSENSOR 8
extern SensorDef sensors[MAXSENSOR];