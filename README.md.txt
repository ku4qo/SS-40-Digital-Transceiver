/* Firmware for SS-40+ transceiver
* by Jim Kortge K8IQY and Mike Maiorana KU4QO
* Press and hold enter on power up to start calibration
* si5351 clock 0 is vfo frequency and clock 2 is transmit frequency;  
* clock 0 is temporarily used for calibrating the 25 MHz si5351 clock during the clock calibration routine.
* This version uses a 4x20 I2C LCD display.  Calibration routines use the enter button to advance.
* Use si5351mcu low phase noise library - by Pavel Milanes, CO7WT https://github.com/pavelmc/Si5351mcu
* IF is centered at 11.000150 MHz for this rig, VFO is above IF, 18.030150 MHz for 7.030 Mhz receive USB, or 3.97 MHz for LSB.
* Interrupt-based Rotary Encoder based on code by Simon Merrett, based on insight from Oleg Mazurov, Nick Gammon, rt, Steve Spence
* UP_BTN is left-most switch and increases tuning step size, DOWN_BTN is right-most switch and decreases tuning step size
* Long press of UP_BTN switches sidebands (cw to cwr)
* Long press of DN_BTN saves current frequency as startup frequency
* Short press of encoder button toggles LCD backlight
* Long press of encoder button enters keyer speed menu
* Short press of Enter key enters RIT mode
* Long press of Enter key enters tune mode (turns on transmitter for 5 seconds or until you press the enter key again)              
* 
*/