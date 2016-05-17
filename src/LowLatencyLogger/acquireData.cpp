#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library

#include "arduino.h"
#include "myAssert.h"
#include "tca9548a_i2c_multiplexer.h"
#include "writeBuffer.h"
#include "acquireData.h"
#include "sensorDetect.h"

MMA8452Q  MM8452Q_0x1c(0x1C), MM8452Q_0x1d(0x1D);	// i2c adress as constructor
int MM8452Q_0x1c_init = false, MM8452Q_0x1d_init = false, acquireInit = false;
unsigned long acquireLoop, acquireDuration, sensorWaitAVG, syncWaitAVG, flushDurationAVG, flushes, sdBusyWaitAVG, switchTimeAVG, sensorLoopAVG, sdWriteAVG, sensorReadAVG, acquireDataAVG, sensorPrintAVG;

void timerStats()
{
	Serial.println();
	Serial.print(F("acquireData()/Line: "));
	Serial.println(acquireDuration / acquireLoop);
	Serial.print(F("us/Loop: "));
	Serial.println(sensorLoopAVG/acquireLoop);
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

//#define DEBUG_ACQUIREDATA
#ifdef DEBUG_ACQUIREDATA
#define DEBUG_ACQUIREDATA_INIT
#endif 



void acquireData(WriteBuffer *wb) {
	if (!acquireInit)
	{
		wb->print(F("MicroSeconds"));
		acquireInit = true;
	}
	else 
		wb->print(micros());
	int i = 0;
	while (i < 8)
	{
		auto sensorLoopTimer = micros();
#ifdef DEBUG_ACQUIREDATA
		Serial.print(F("Sensor #"));
		Serial.print(i);
		Serial.print(F(" ..."));
		Serial.flush();
#endif
		int tcaChannel = sensors[i].tcaChannel;
		if ( tcaChannel == TCA_INVALID)
			break;
#ifdef DEBUG_ACQUIREDATA
		Serial.print(F(" SwitchI2c("));
		Serial.print(tcaChannel);
		Serial.print(F(") ..."));
		Serial.flush();
#endif
		auto switchTimer = micros();
		tca9548a.switchI2C(tcaChannel);
		switchTimeAVG += micros() - switchTimer;
		wb->write(';');
		auto acquireDataTimer = micros();
		sensors[i].acquireDataCSV(wb);
		acquireDataAVG += micros() - acquireDataTimer;
		i++;
		sensorLoopAVG += micros() - sensorLoopTimer;
	}
	wb->write('\n');
	acquireLoop++;
#ifdef DEBUG_ACQUIREDATA
	Serial.println(F(" DONE");
	Serial.flush();
#endif
}

void MM8452Q_AcquireDataCSV(MMA8452Q &accel, WriteBuffer *wb)
{
#ifdef DEBUG_ACQUIREDATA
	Serial.print(F("MM8452Q available ..."));
	Serial.flush();
#endif
	auto waitingTimer = micros();
	if (accel.available())
	{
		sensorWaitAVG += micros() - waitingTimer;
#ifdef DEBUG_ACQUIREDATA
		Serial.print(F(" read ..."));
		Serial.flush();
#endif
		auto sensorReadTimer = micros();
		accel.read();
		sensorReadAVG += micros() - sensorReadTimer;
		auto sensorPrintTimer = micros();
		wb->itoa(accel.x);
		wb->write(';');
		wb->itoa(accel.y);
		wb->write(';');
		wb->itoa(accel.z);
		sensorPrintAVG += micros() - sensorPrintTimer;
	}
#ifdef DEBUG_ACQUIREDATA
	Serial.println(F(" DONE"));
	Serial.flush();
#endif
}

void MM8452Q_0x1c_AcquireDataCSV(WriteBuffer *wb)
{
	if (!MM8452Q_0x1c_init)
	{
#ifdef DEBUG_ACQUIREDATA_INIT
		Serial.print(F("MM8452Q_0x1c.init ..."));
		Serial.flush();
#endif
		assert(MM8452Q_0x1c.init(SCALE_8G,ODR_400));
		wb->print(F("MM8452Q_0x1c X;MM8452Q_0x1c Y;MM8452Q_0x1c Z"));
		MM8452Q_0x1c_init = true;
#ifdef DEBUG_ACQUIREDATA_INIT
		Serial.println(F(" DONE");
		Serial.flush();
#endif
		wireInit();
		return;
	}
	MM8452Q_AcquireDataCSV(MM8452Q_0x1c, wb);
}

void MM8452Q_0x1d_AcquireDataCSV(WriteBuffer *wb)
{
	if (!MM8452Q_0x1d_init)
	{
#ifdef DEBUG_ACQUIREDATA_INIT
		Serial.print(F("MM8452Q_0x1c.init ..."));
		Serial.flush();
#endif
		assert(MM8452Q_0x1d.init(SCALE_8G, ODR_400));
		wb->print(F("MM8452Q_0x1d X;MM8452Q_0x1d Y;MM8452Q_0x1d Z"));
		MM8452Q_0x1d_init = true;
#ifdef DEBUG_ACQUIREDATA_INIT
		Serial.println(F(" DONE");
		Serial.flush();
#endif
		wireInit();
		return;
	}
	MM8452Q_AcquireDataCSV(MM8452Q_0x1d, wb);
}

void resetAcquireMetrics()
{
	acquireLoop = 0; sensorWaitAVG = 0; syncWaitAVG = 0; sdBusyWaitAVG = 0; switchTimeAVG = 0; sdWriteAVG = 0; flushes = 0; sensorReadAVG = 0; acquireDataAVG = 0; sensorPrintAVG = 0;
	acquireDuration = micros();
	flushDurationAVG = 0;
	MM8452Q_0x1c_init = false; MM8452Q_0x1d_init = false; acquireInit = false;
}