#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library
#include <SdFat.h> // already have custom hardware SPI implementation
SdFat SD;

#define __ASSERT_USE_STDERR
#include <assert.h>
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
    Serial.println("Assertion Fail:");
    Serial.println(__func);
    Serial.println(__file);
    Serial.println(__lineno, DEC);
    Serial.println(__sexp);
    Serial.flush();
    abort();
}

// Begin using the library by creating an instance of the MMA8452Q
//  class. We'll call it "accel". That's what we'll reference from
//  here on out.
MMA8452Q
  accel1(0x1D)  // i2c adress passed to constructor
  ,accel2(0x1C);  // i2c adress passed to constructor

unsigned long recordStart,nextSampleWait,loopCount,recordCount;
bool recordState = false, serialForceRecording = false;
#define RECORDPIN 2
#define SDCARDCSPIN 10

typedef ofstream FileType;
FileType myFile;

void setup()
{ 
  pinMode(RECORDPIN, INPUT);  // Record a session when RECORDPIN is low.
  digitalWrite(RECORDPIN, HIGH);  // enable pullup on RECORDPIN
  
  Serial.begin(9600);
  Serial.println("Init.");
  assert(accel1.init(SCALE_8G,ODR_400));
  Serial.println("MMA8452Q #1 OK.");
  assert(accel2.init(SCALE_8G,ODR_400));
  Serial.println("MMA8452Q #2 OK.");
  if (!SD.begin(10,1))
  {
    Serial.println("Please insert SDcard.");
    delay(1000);
  }
  while(!SD.begin(10,1))
    delay(1000);
  Serial.println("SDcard OK.");

  // TWBR change i2c frequency according to WIRE library documentation
  TWBR = 12; // #define TWI_FREQ 400000L
  // TWBR = 2; // #define TWI_FREQ 800000L

  nextSampleWait = 0;
  loopCount = 0;
  recordCount = 0;
}

// The loop function will simply check for new data from the
//  accelerometer and print it out if it's available.
void loop()
{
  #define MICRODELAY 250

  if (Serial.available())
  {
    Serial.print("Serial recording toggle.\n");
    Serial.read();
    serialForceRecording ^= 1;  // reverse state
  }

  if (digitalRead(RECORDPIN) == 0 || serialForceRecording)  // RECORDPIN low (=0) => enable recording
  {
    if (!recordState)
    {
      Serial.print("Recording ENABLED.");
      Serial.println();
      recordState = false;
      openNewFile();
      recordStart = millis();
    }
    recordState = true;
    recordLoop();
  }
  else  // RECORDPIN pulled-up => stop recording
  {
    if (recordState)
    {
      recordStart = millis() - recordStart;
      Serial.print("Recording DISABLED.");
      Serial.println();
      recordState = false;
      closeCurrentFile();
      benchmark();
    }
    delay(500);
  }
}

void openNewFile()
{
  SD.remove("datalog.csv");
  myFile.open("datalog.csv");  // http://forum.arduino.cc/index.php?topic=49649.0
  assert(myFile.is_open());
  myFile << "millis;X1;Y1;Z1;X2;Y2;Z2\r\n";  // csv header
}

void closeCurrentFile()
{
  myFile.close();
  assert(!myFile.is_open());
}

void recordLoop()
{
  unsigned long now = micros();
  while (!accel1.available())
    delayMicroseconds(MICRODELAY);
  while (!accel2.available())
    delayMicroseconds(MICRODELAY);
  nextSampleWait += micros() - now;
  accel1.read();
  accel2.read();

  myFile << now << ';' << accel1.x << ';' << accel1.y << ';' << accel1.z << ';' << accel2.x << ';' << accel2.y << ';' << accel2.z << "\r\n";   // https://forums.adafruit.com/viewtopic.php?f=31&t=20951

  recordCount++;
  loopCount++;
}

void benchmark()
{
  printCalculatedAccels(accel1);
  printOrientation(accel1);
  Serial.print("\t");
  printCalculatedAccels(accel2);
  printOrientation(accel2);
  Serial.println();

  Serial.print("RecordCount: ");
  Serial.print(recordCount);
  Serial.println();

  Serial.print("CPU use: ");
  Serial.print(2500 - nextSampleWait/loopCount); // 2500micros available per sample
  Serial.print(" of 2500micros");
  Serial.println();
  
  Serial.print("Samples per second:");
  Serial.print((double)(1000)*loopCount/recordStart);
  Serial.print("Hz");
  Serial.println();
  Serial.println();
  
  loopCount = 0;
  nextSampleWait = 0;
}

// The function demonstrates how to use the accel.x, accel.y and
//  accel.z variables.
// Before using these variables you must call the accel.read()
//  function!
void printAccels( const MMA8452Q & accel )
{
  Serial.print(accel.x, 3);
  Serial.print("\t");
  Serial.print(accel.y, 3);
  Serial.print("\t");
  Serial.print(accel.z, 3);
  Serial.print("\t");
}

// This function demonstrates how to use the accel.cx, accel.cy,
//  and accel.cz variables.
// Before using these variables you must call the accel.read()
//  function!
void printCalculatedAccels(const MMA8452Q & accel)
{ 
  Serial.print(accel.cx, 3);
  Serial.print("\t");
  Serial.print(accel.cy, 3);
  Serial.print("\t");
  Serial.print(accel.cz, 3);
  Serial.print("\t");
}

// This function demonstrates how to use the accel.readPL()
// function, which reads the portrait/landscape status of the
// sensor.
void printOrientation(MMA8452Q & accel)
{
  // accel.readPL() will return a byte containing information
  // about the orientation of the sensor. It will be either
  // PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L, or
  // LOCKOUT.
  byte pl = accel.readPL( );
  switch (pl)
  {
  case PORTRAIT_U:
    Serial.print("Portrait Up");
    break;
  case PORTRAIT_D:
    Serial.print("Portrait Down");
    break;
  case LANDSCAPE_R:
    Serial.print("Landscape Right");
    break;
  case LANDSCAPE_L:
    Serial.print("Landscape Left");
    break;
  case LOCKOUT:
    Serial.print("Flat");
    break;
  }
}
