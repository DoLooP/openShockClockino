# openShockClockino
arduino uno datalogger project to measure shock metrics while ridding

hardware:
 - 1x usb power battery
 - 1x arduino uno R2
 - 1x adafruit shield SDcard
 - 2x mma8452 accelerometer
 - 1x lsm303 accelerometer + compass
 - 1x 10Dof IMU based on lsm303
 - 2x 200mm http://www.spectrasymbol.com/potentiometer/magnetopots

Project steps:
 - connect to mma8452 through i2c /step1 DONE/
 - connect/sample dual mma8452 through i2c /step2 DONE/ ( max sample rate: 340Hz :/ )
 - improve sampling rate (verify i2c clock, read MMA8452 lib) /step3 DONE/
     > (increased i2c clock from 100 to 400 / 800KHz => max Sample rate is now 800hz, limited by MMA8452 max output)
 - assemble SDcard shield /DONE/
 - Test SDCARD lib /step4 DONE/
 - init realtime clock dialog
 - output realtime-clock column in csv file @ 400hz   *<<-- WE'RE HERE !*
 - clock;mma8452 >> csv file @ 400hz
 - Add a lsm303 on i2c
 - clock;mma8452;lsm303 >> csv file @ 400hz
 - add a spectraSymbol
 - build a slider for the magnetopots
 - adds all other sensors
...
Maybe we'll hit i2c bandwidth limitation before interfacing 4 digitals sensors. In this case we'll use some kind of SDI to i2c switch.

project goals:
Output data file as jsonP scripts + index.htm (=>record list)
jsonP files are then loaded by a javascript engine (localy stored in the sdcard) in order to:
 - export as calc-sheets
 - display as JSchart
 - measure shock displacement coverage
 - detect bottoming
 - detect traction lose
 - detect rider comfort
 - compare rides
