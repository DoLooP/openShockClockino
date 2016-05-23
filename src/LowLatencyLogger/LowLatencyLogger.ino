/*
	HACKED through SdFat LowLatencyLogger exemple
*/
#include "Arduino.h"
#include <Wire.h>
#include <SparkFun_MMA8452Q.h>
#include <Adafruit_LSM303.h>
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include "LLLconstant.h"
#include "myAssert.h"
#include "writeBuffer.h"
#include "acquireData.h"
#include "sensorDetect.h"

SdFat sd;
SdBaseFile binFile;

void fatalBlink() {
	while (true) {
		if (ERROR_LED_PIN >= 0) {
			digitalWrite(ERROR_LED_PIN, HIGH);
			delay(200);
			digitalWrite(ERROR_LED_PIN, LOW);
			delay(200);
		}
	}
}

//------------------------------------------------------------------------------
// log data
// max number of blocks to erase per erase call
uint8_t* cache;

//#define DEBUG_FLUSHCACHE
void flushCache() {
	flushes++;
#ifdef DEBUG_FLUSHCACHE 
	Serial.println(F("Waiting sdcard...")); Serial.flush();
#endif
	auto sdBusy = micros();
	while (sd.card()->isBusy());
	sdBusyWaitAVG += micros() - sdBusy;
#ifdef DEBUG_FLUSHCACHE
	Serial.println(F("SdCard writeData...")); Serial.flush();
#endif
	auto sdWriteTimer = micros();
	assert(sd.card()->writeData(cache));
	sdWriteAVG += micros() - sdWriteTimer;
#ifdef DEBUG_FLUSHCACHE
	Serial.println(F("flushCache() DONE")); Serial.flush();
#endif
}

uint32_t const ERASE_SIZE = 262144L;
void logData() {
  uint32_t bgnBlock, endBlock;
  Serial.println();

  // Find unused file name.
  assert(BASE_NAME_SIZE <= 6);
  while (sd.exists(binName)) {
    if (binName[BASE_NAME_SIZE + 1] != '9') {
      binName[BASE_NAME_SIZE + 1]++;
    } else {
      binName[BASE_NAME_SIZE + 1] = '0';
	  assert(binName[BASE_NAME_SIZE] < '9');
      binName[BASE_NAME_SIZE]++;
    }
  }
  // Delete old tmp file.
  if (sd.exists(TMP_FILE_NAME)) {
    Serial.println(F("Deleting tmp file"));
	assert(sd.remove(TMP_FILE_NAME));
  }
  // Create new file.
  Serial.println(F("Creating new file"));
  binFile.close();
  assert(binFile.createContiguous(sd.vwd(), TMP_FILE_NAME, 512 * FILE_BLOCK_COUNT));
  // Get the address of the file on the SD.
  assert(binFile.contiguousRange(&bgnBlock, &endBlock));
  // Use SdFat's internal buffer.
  cache = (uint8_t*)sd.vol()->cacheClear();
  assert(cache != 0);
  WriteBuffer wb(cache, 512 , flushCache);
  // Flash erase all data in the file.
  Serial.println(F("Erasing all data"));
  uint32_t bgnErase = bgnBlock;
  uint32_t endErase;
  while (bgnErase < endBlock) {
    endErase = bgnErase + ERASE_SIZE;
    if (endErase > endBlock) {
      endErase = endBlock;
    }
	assert(sd.card()->erase(bgnErase, endErase));
    bgnErase = endErase + 1;
  }
  // Start a multiple block write.
  assert(sd.card()->writeStart(bgnBlock, FILE_BLOCK_COUNT));

  // **************************************************************************** contiguous file init end

  Serial.println(F("Logging - type any character to stop"));
  // Wait for Serial Idle.
  Serial.flush();
  delay(10);

  int32_t diff;
  // Start at a multiple of interval.
  uint32_t logTime = micros()/LOG_INTERVAL_USEC + 1;
  logTime *= LOG_INTERVAL_USEC;

  resetAcquireMetrics();
  unsigned long lastAcquireTick;
  while (1) {
	if (Serial.available())
		break;

	auto tick = micros();
	syncWaitAVG -= tick;
	long toWait = LOG_INTERVAL_USEC - (tick - lastAcquireTick);
	if (toWait > 0)
		delayMicroseconds(toWait);
	lastAcquireTick = micros();
	syncWaitAVG += lastAcquireTick;
	acquireData(&wb);
  }
  acquireDuration += micros();
  sdTotalWrite = wb.totalWriten;

  Serial.println(F("Flush..."));
  wb.flush();
  Serial.println(F("Stop..."));
  assert(sd.card()->writeStop());
  Serial.print(F("Truncate to "));
  Serial.print(wb.totalWriten >> 10);
  Serial.print("Ko ...");
  assert(binFile.truncate(wb.totalWriten));
  wb.totalWriten = 0;
  Serial.println(F("Rename..."));
  assert(binFile.rename(sd.vwd(), binName));
  Serial.println(F("Done"));
}
//------------------------------------------------------------------------------
void setup(void) {
  if (ERROR_LED_PIN >= 0) {
    pinMode(ERROR_LED_PIN, OUTPUT);
  }
  Serial.begin(9600);
  while (!Serial) {}

  Serial.print(F("FreeRam: "));
  Serial.println(FreeRam());
  // initialize file system.
  if (!sd.begin(SD_CS_PIN, SPI_FULL_SPEED)) {
    sd.initErrorPrint();
    fatalBlink();
  }

  
  sensorDetect();
  // TWBR change i2c frequency according to WIRE library documentation
  // TWBR = 2; // #define TWI_FREQ 800000L

  Serial.print(F("Init OK"));
}
//------------------------------------------------------------------------------
void loop(void) {
    while (Serial.read() >= 0) {delay(100);}
    while (!Serial.available()) {delay(100);}
    Serial.read();
    logData();
	timerStats();
}
