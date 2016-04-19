#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library
// include the SDfat library:
#include <SPI.h>
#include <SdFat.h>
SdFat SD;

#define __ASSERT_USE_STDERR
#include <assert.h>
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
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

unsigned long lastMillis,nextSampleWait;
unsigned long loopCount;
bool recordState = false, serialForceRecording = false;
#define RECORDPIN 2
#define SDCARDCSPIN 10

unsigned long recordCount;
File myFile;

class WriteBuffer : public Print
{
  public:
  uint8_t str[64];
  uint8_t *p;
  File *file;

  WriteBuffer(File &file)
  {
    this->file = &file;
    reset();
  }

  void flush()
  {
    assert(p > str);
    assert(p-str <= sizeof str);
    file->write(str,reset());
  }

  size_t reset()
  {
    size_t s = p-str;
    p = str;
    return s;
  }

  virtual size_t write(const uint8_t *buffer, size_t l)
  {
    int sizeUsed = p-str;
    while (sizeUsed+l > sizeof str)
    {
      int sizeLeft = sizeof str - sizeUsed;
      memcpy(p,buffer,sizeLeft);
      flush();
      buffer += sizeLeft;
      l -= sizeLeft;
      sizeUsed = 0;
    }
    assert( l<sizeof str );
    memcpy(p,buffer,l);
    p += l;
    return p-str;
  }
  
  virtual size_t write(uint8_t c)
  {
    if (p-str >= sizeof str)
      flush();
    *p = c;
    p++;
    return p-str;
  }
} wb(myFile);

/*
struct {
  unsigned long 
    outsideLoop
    ,accelAvailable
    ,accelRead
    ,sdWrite;
} profiler;
*/

void setup()
{ 
  pinMode(RECORDPIN, INPUT);  // Record a session when RECORDPIN is low.
  digitalWrite(RECORDPIN, HIGH);  // enable pullup on RECORDPIN
  
  Serial.begin(9600);
  Serial.println("Dual MMA8452Q init.");
  
  // Choose your adventure! There are a few options when it comes
  // to initializing the MMA8452Q:
  //  1. Default init. This will set the accelerometer up
  //     with a full-scale range of +/-2g, and an output data rate
  //     of 800 Hz (fastest).
  // accel.init();
  //  2. Initialize with FULL-SCALE setting. You can set the scale
  //     using either SCALE_2G, SCALE_4G, or SCALE_8G as the value.
  //     That'll set the scale to +/-2g, 4g, or 8g respectively.
  //accel.init(SCALE_4G); // Uncomment this out if you'd like
  //  3. Initialize with FULL-SCALE and DATA RATE setting. If you
  //     want control over how fast your accelerometer produces
  //     data use one of the following options in the second param:
  //     ODR_800, ODR_400, ODR_200, ODR_100, ODR_50, ODR_12,
  //     ODR_6, or ODR_1. 
  //     Sets to 800, 400, 200, 100, 50, 12.5, 6.25, or 1.56 Hz.
  //accel.init(SCALE_8G, ODR_6);
  accel1.init(SCALE_8G,ODR_400);
  accel2.init(SCALE_8G,ODR_400);
  Serial.println("Dual MMA8452Q init done.");

  //sdcardSetup();
  SD.begin(SDCARDCSPIN);

  // TWBR change i2c frequency according to WIRE library documentation
  TWBR = 12; // #define TWI_FREQ 400000L
  // TWBR = 2; // #define TWI_FREQ 800000L

  lastMillis = micros()/1000; // to print a debug msg every N ms.
  nextSampleWait = 0;
  loopCount = 0;
  recordCount = 0;
}

// The loop function will simply check for new data from the
//  accelerometer and print it out if it's available.
void loop()
{
  #define MICRODELAY 250
  #define BETWEENMSG 2000

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
    }
    recordState = true;
    recordLoop();
  }
  else  // RECORDPIN pulled-up => stop recording
  {
    if (recordState)
    {
      Serial.print("Recording DISABLED.");
      Serial.println();
      recordState = false;
      wb.flush();
      closeCurrentFile();
    }
    delay(500);
  }

}

void openNewFile()
{
  SD.remove("datalog.csv");
  myFile = SD.open("datalog.csv", FILE_WRITE);
  myFile.write("millis;X1;Y1;Z1;X2;Y2;Z2\r\n");  // csv header
}

void closeCurrentFile()
{
  myFile.close();
}

void recordLoop()
{
  long now = micros();
  while (!accel1.available())
    delayMicroseconds(MICRODELAY);
  while (!accel2.available())
    delayMicroseconds(MICRODELAY);
  nextSampleWait += micros() - now;
  accel1.read();
  accel2.read();

  wb.print(now);
  wb.write(';');
  wb.print(accel1.x);
  wb.write(';');
  wb.print(accel1.y);
  wb.write(';');
  wb.print(accel1.z);
  wb.write(';');
  wb.print(accel2.x);
  wb.write(';');
  wb.print(accel2.y);
  wb.write(';');
  wb.print(accel2.z);
  wb.println();

  recordCount++;
  loopCount++;

  now /= 1000;  // micros to millis
  if ( now - lastMillis > BETWEENMSG)
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
    Serial.print((double)(1000)*loopCount/BETWEENMSG);
    Serial.print("Hz");
    Serial.println();
    Serial.println();
    
    loopCount = 0;
    nextSampleWait = 0;
    lastMillis = micros()/1000;
  }
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
