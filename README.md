# SS-40-plus
The SS-40-plus project is a high-performance transceiver built for 40 meters.
This is a "quarantine" project designed by Jim Kortge K8IQY with Mike Maiorana KU4QO.
The basis of the receiver is Jim's excellent SS-40 receiver. 
The transceiver is built with separate functional modules. 
* Receiver
* Digital VFO and Control
* Transmitter and PIN switching
* Input Attenuator

The receiver is based on the ADE-1 mixer with an NE602 product detector. There is a 7-element IF filter at 11MHz. 
There is a very effective AGC that controls both the input RF amp and the IF signal level. The receiver is exceptionally quiet and sensitive.

The transmitter is a class-E design similar to that in the QRP Labs QCX. It provides 4 to 5 watts out with a 12v supply that is easily within FCC purity requirements. It also uses a PIN diode T/R switch that is sequenced by the CPU, which greatly reduces big-signal intermodulation artifacts of simple diode-switched transceivers.

The Arduino NANO based Digital Control board provides agile VFO and transmit signals, as well as an iambic keyer, sideband switching, RIT and T/R control.

The modules can be built as needed to create different configurations.

The receiver can be built alone and driven with an external VFO.

The attenuator can be used on other transceivers.

The digital VFO/control can be used stand-alone as well.

The transmitter/PIN board can be used to add a CW transmitter to an existing receiver.

This project is being released to encourage experimentation. It is released under the license listed on this github page. 

The firmware, as well as the schematics and gerber files for the boards are contained in this repository.

# Firmware for the SS-40+ transceiver project
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
* Long press of Enter key enters tune mode (turns on transmitter for 10 seconds or until you press the enter key again)              

# The information contained here is provided without support or warranty, as stated in the license details.
