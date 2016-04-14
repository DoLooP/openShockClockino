/******************************************************************************
MMA8452Q_Basic.ino
SFE_MMA8452Q Library Basic Example Sketch
Jim Lindblom @ SparkFun Electronics
Original Creation Date: June 3, 2014
https://github.com/sparkfun/MMA8452_Accelerometer

This sketch uses the SparkFun_MMA8452Q library to initialize the
accelerometer, and stream values from it.

Hardware hookup:
  Arduino --------------- MMA8452Q Breakout
    3.3V  ---------------     3.3V
    GND   ---------------     GND
  SDA (A4) --\/330 Ohm\/--    SDA
  SCL (A5) --\/330 Ohm\/--    SCL

The MMA8452Q is a 3.3V max sensor, so you'll need to do some 
level-shifting between the Arduino and the breakout. Series
resistors on the SDA and SCL lines should do the trick.

Development environment specifics:
	IDE: Arduino 1.0.5
	Hardware Platform: Arduino Uno
	
	**Updated for Arduino 1.6.4 5/2015**

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/
#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library

// Begin using the library by creating an instance of the MMA8452Q
//  class. We'll call it "accel". That's what we'll reference from
//  here on out.
MMA8452Q
  accel1(0x1D)  // i2c adress passed to constructor
  ,accel2(0x1C);  // i2c adress passed to constructor

// The setup function simply starts serial and initializes the
//  accelerometer.
unsigned long lastMillis,duration;
unsigned long loopCount;

void setup()
{
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
  accel1.init(SCALE_8G);
  accel2.init(SCALE_8G);
  Serial.println("Dual MMA8452Q init done.");

  lastMillis = millis(); // to print a debug msg every N ms.
  duration = 0;
  loopCount = 0;
}

// The loop function will simply check for new data from the
//  accelerometer and print it out if it's available.
void loop()
{
  #define MICRODELAY 500
  #define BETWEENMSG 5000
  #define HZCOEFF (BETWEENMSG*0.001)

  long start = micros();
  {
    while (!accel1.available())
      delayMicroseconds(MICRODELAY);
    accel1.read();
    while (!accel2.available())
      delayMicroseconds(MICRODELAY);
    accel2.read();
  }
  duration += micros() - start;
  loopCount++;
  
  // #define DEBUG
  #ifdef DEBUG
    Serial.print("Dual accel read:");
    Serial.print(duration);
    Serial.println();
    delay (333);
  #endif

  long now = millis();
  if ( now - lastMillis > BETWEENMSG)
  {
    lastMillis = now;
    Serial.print("Accel 1:");
    printCalculatedAccels(accel1);
    printOrientation(accel1);
    Serial.println();

    Serial.print("Accel 2:");
    printCalculatedAccels(accel2);
    printOrientation(accel2);
    Serial.println();

    int avgDuration = duration / loopCount;
    Serial.print("Loops:");
    Serial.print(loopCount);
    Serial.println();

    Serial.print("Sampling duration:");
    Serial.print(avgDuration);
    Serial.print("microseconds");
    Serial.println();
    
    Serial.print("Samples per second:");
    Serial.print((double)1000000/avgDuration);
    Serial.print("Hz");
    Serial.println();
    Serial.println();
    
    loopCount = 0;
    duration = 0;
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
