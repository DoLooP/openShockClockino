#include "Arduino.h"
#include <Wire.h> // Must include Wire library for I2C
#include "myAssert.h"
#include "writeBuffer.h"
#include "acquireMMA.h"
#include "acquireLSM.h"

void acquireData(WriteBuffer *);
void resetAcquireMetrics();
void timerStats();
void wireInit();

extern unsigned long acquireLoop, acquireDuration, sdTotalWrite, syncWaitAVG, flushDurationAVG, flushes, sdBusyWaitAVG,
	sdWriteAVG, switchTimeAVG, sensorWaitAVG, sensorReadAVG, sensorPrintAVG;