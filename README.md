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

project goal
output data file as jsonP scripts + index.htm
data file are loaded by a local javascript engine (stored in the sdcard)
 - exported as calc-sheets
 - displayed as JSchart
 - measure shock displacement coverage
 - detect bottoming
 - detect traction lose
 - detect rider comfort
 - compare rides
