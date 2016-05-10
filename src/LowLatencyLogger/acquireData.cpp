#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library

#include "arduino.h"
#include "writeBuffer.h"
#include "acquireData.h"
#include "sensorDetect.h"

void acquireData(WriteBuffer &wb) {
	wb.print("");
}