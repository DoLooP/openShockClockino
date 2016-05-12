void sensorDetect();

class WriteBuffer;
typedef void (*SensorAcquireDataCSV)(WriteBuffer *);

struct SensorDef {
	int tcaChannel;
	SensorAcquireDataCSV acquireDataCSV;
};

#define MAXSENSOR 8
extern SensorDef sensors[MAXSENSOR];