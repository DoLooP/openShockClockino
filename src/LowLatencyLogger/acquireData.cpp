#include "Arduino.h"
#include "tca9548a_i2c_multiplexer.h"
#include "acquireData.h"
#include "sensorDetect.h"

unsigned long acquireLoop, acquireDuration, sensorWaitAVG, syncWaitAVG, flushDurationAVG, flushes, sdBusyWaitAVG, switchTimeAVG, sensorLoopAVG, sdWriteAVG, sensorReadAVG, acquireDataAVG, sensorPrintAVG, sdTotalWrite;

void timerStats()
{
	Serial.println();
	Serial.print(F("Ko/s: "));
	Serial.println(sdTotalWrite / (acquireDuration/976));
	Serial.print(F("acquireData()/Line: "));
	Serial.println(acquireDuration / acquireLoop);
	Serial.print(F("us/Loop: "));
	Serial.println(sensorLoopAVG/acquireLoop);
	Serial.print(F("Sync wait/Line: "));
	Serial.println(syncWaitAVG / acquireLoop);
	Serial.print(F("Sensor acquire/Line: "));
	Serial.println(acquireDataAVG / acquireLoop);
	Serial.print(F("Switch I2c/Line: "));
	Serial.println(switchTimeAVG / acquireLoop);
	Serial.print(F("Sensor Wait/Line: "));
	Serial.println(sensorWaitAVG / acquireLoop);
	Serial.print(F("Sensor Read/Line: "));
	Serial.println(sensorReadAVG / acquireLoop);
	Serial.print(F("Sensor Print/Line: "));
	Serial.println(sensorPrintAVG / acquireLoop);
	Serial.print(F("Sample/flush: "));
	Serial.println(acquireLoop / flushes);
	Serial.print(F("sdBusyWait/flush: "));
	Serial.println(sdBusyWaitAVG / flushes);
	Serial.print(F("sdWrite/flush: "));
	Serial.println(sdWriteAVG / flushes);
	Serial.print(F("sdBusyWait/Line: "));
	Serial.println(sdBusyWaitAVG / acquireLoop);
	Serial.print(F("sdwrite/Line: "));
	Serial.println(sdWriteAVG / acquireLoop);
	Serial.println();
}

void wireInit()
{
	TWBR = 12; // #define TWI_FREQ 400000L
	//TWBR = 2; // #define TWI_FREQ 800000L
}

#define DEBUG_ACQUIREDATA
#ifdef DEBUG_ACQUIREDATA
//#define DEBUG_ACQUIREDATA_OUTPUT
//#define DEBUG_MMA
#define DEBUG_LSM_INIT
#endif 



void acquireData(WriteBuffer *wb) {
	if (acquireLoop == 0)
		wb->print(F("MicroSeconds"));
	else 
		wb->itoa(micros());
	int i = 0;
	while (i < 8)
	{
		auto tcaChannel = sensors[i].tcaChannel;
		if (tcaChannel == TCA_INVALID)
			break;

		auto tick = micros();
		sensorLoopAVG -= tick;
#ifdef DEBUG_ACQUIREDATA_OUTPUT
		Serial.print(F("Sensor #"));
		Serial.print(i);
		Serial.print(F(" ..."));
		Serial.flush();
#endif
		switchTimeAVG -= tick;
#ifdef DEBUG_ACQUIREDATA_OUTPUT
		Serial.print(F(" SwitchI2c("));
		Serial.print(tcaChannel);
		Serial.print(F(") ..."));
		Serial.flush();
#endif
		tca9548a.switchI2C(tcaChannel);
		tick = micros();
		switchTimeAVG += tick;
		acquireDataAVG -= tick;
		wb->write(';');
		sensors[i].acquireDataCSV(wb);
		tick = micros();
		acquireDataAVG += tick;
		i++;
		sensorLoopAVG += tick;
	}
	wb->write('\n');
	if (acquireLoop == 0)
		wireInit();
	acquireLoop++;
#ifdef DEBUG_ACQUIREDATA_OUTPUT
	Serial.println(F(" DONE");
	Serial.flush();
#endif
}

void resetAcquireMetrics()
{
	acquireLoop = 0; sensorWaitAVG = 0; syncWaitAVG = 0; sdBusyWaitAVG = 0; switchTimeAVG = 0; sdWriteAVG = 0; flushes = 0; sensorReadAVG = 0; acquireDataAVG = 0; sensorPrintAVG = 0; sensorLoopAVG = 0;
	acquireDuration = -micros();
	flushDurationAVG = 0;
}