# openShockClockino
arduino uno datalogger project to measure shock metrics while ridding

hardware:
1x usb power battery
1x arduino uno R2
1x adafruit shield SDcard
2x mma8452 accelerometer
1x lsm303 accelerometer + compass
1x 10Dof IMU based on lsm303
2x 200mm http://www.spectrasymbol.com/potentiometer/magnetopots

Project steps:
 - assemble SDcard shield <<-- WE'RE HERE !
 - init realtime clock dialog
 - output realtime-clock column in csv file @ 400hz
 - connect to mma8452 through i2c
 - clock;mma8452 >> csv file @ 400hz
 - Add a lsm303 on i2c
 - clock;mma8452;lsm303 >> csv file @ 400hz
 - add a spectraSymbol
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
