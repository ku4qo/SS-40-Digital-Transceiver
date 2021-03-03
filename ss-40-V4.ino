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

#include <si5351mcu.h>          //Use Pavel low phase noise library
#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

#define ENCODER_BTN   4       //Encoder pushbutton
#define DOWN_BTN      5       //Down button
#define UP_BTN        6       //Up button
#define ENTER_BTN     7       //Enter button
#define RX_MUTE       8       //Rx mute line, low to mute
#define XMIT_PWR      13      //Tx power to finals, high to enable
#define TX_PIN        10      //Tx PIN switch control, high to enable
#define RX_PIN        11      //Rx PIN switch control, high to enable
#define DIT_KEY       12      //Dit key contact
#define DAH_KEY       9       //Dah key contact

#define A0            A0      //spare
#define A1            A1      //spare
#define A2            A2      //spare
#define XMIT_EN       A3      //Clock 2 frequency enable
#define SDA           A4      //I2C data
#define SCL           A5      //I2C clock
#define PS_MONITOR    A7      //Power supply monitor

#define EE_SAVED_FQ     0     // spot to save tuned frequency 
#define EE_SAVED_CAL    4     // si5351 calibration value
#define EE_SAVED_IF     8     // calibrated IF frequency
#define EE_KEYER_SPEED  12    // spot to save keyer speed

//keyercontrol flags
#define     DIT_L      0x01     // Dit latch
#define     DAH_L      0x02     // Dah latch
#define     DIT_PROC   0x04     // Dit is being processed
#define     PDLSWAP    0x08     // 0 for normal, 1 for swap
#define     IAMBICB    0x10     // 0 for Iambic A, 1 for Iambic B

#define     if_guess 11000250   //estimate of IF frequency center freq. - used for uncalibrated device

//setup for clock generation control
long if_freq=11000150; //actual IF frequency, used for calculations
long fq=7030000;       //initial startup frequency
long vfo;
long cal_ofst=0;
char LCDstr[14];       //Holds the displayed frequency

int step_array[6]={1, 10, 100, 500, 1000, 10000}; // step values in Hz
int step_index=3;                                 // initial step value 100Hz

bool rit_state = false;
long rit_freq = 0;        //initial RIT shift in Hz
bool start_cal = false;
bool usb = false;          //set initial sideband for reception
const float vpb=0.0048828;   //volts per bit of ADC
float vbatt;              //battery voltage reading
unsigned long v_time=0;   //time of last ADC read

//setup for keyer
unsigned long       ditTime;                      // No. milliseconds per dit
unsigned char       keyerControl;
unsigned char       keyerState;
enum KSTYPE {IDLE, CHK_DIT, CHK_DAH, KEYED_PREP, KEYED, INTER_ELEMENT };
byte wpm;
unsigned long mt_timer;

bool backlight_state=HIGH;

static long ktimer;             // timer variable for keyer

//setup for interrupt-driven encoder reading
static int pinA = 2; // Our first hardware interrupt pin is digital pin 2
static int pinB = 3; // Our second hardware interrupt pin is digital pin 3
volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent
volatile bool enc_up=0;
volatile bool enc_dn=0;

//Declare hardware being used
Si5351mcu Si;
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the LCD I2C address and display size

// input switches Finite State Machines
unsigned long t_eb = 0;
unsigned long t_en = 0;
unsigned long t_su = 0;
unsigned long t_sd = 0;
unsigned long bounce_delay = 10;
unsigned long hold_delay = 500;
unsigned long tune_timeout = 5000;
enum FSM {RST, WAIT, ARM, DEBOUNCE, LIFT, SHORT, LONG, RELEASE, CANCEL, FIN, FIN_WAIT};
FSM state_eb;  //new variable of enumeration type FSM for encoder button
FSM state_su;  //new variable of enumeration type FSM for switch "up"
FSM state_sd;  //new variable of enumeration type FSM for switch "down"
FSM state_en;  //new variable of enumeration type FSM for switch "enter"

void loop(){ 

  if (start_cal == true) {    //start calibration if enter key was pressed during startup
    wait_key();
    calib();
    start_cal = false;
  }
  
  check_encoder();  //check for encoder rotation

  check_keyer();    //check for keyer operation
    
  fsm_eb(); //call fsm for encoder button functions
    if (state_eb == SHORT) {   //encoder button pressed, toggle backlight
      if (backlight_state == HIGH) {
        backlight_state = LOW;
        lcd.noBacklight();
      }
      else {
        backlight_state = HIGH;
        lcd.backlight();
      }
    }

    if (state_eb == FIN_WAIT) {     //long encoder button, set keyer speed
      adj_keyer();                  //call keyer adjust subroutine
    }

  fsm_su();   //call fsm for up button functions
    if (state_su == SHORT) {
      step_index+=1;
      if (step_index > 5 ) 
      {
       step_index = 0;
      }
      disp_step(fq);
    }
    if (state_su == FIN_WAIT) {         //swap sidebands
      if ( usb == true ) {
        usb = false;
      }
      else {
        usb = true;
      }
      disp_sideband();
      set_frequency(0);
    }

  fsm_sd();   //call fsm for down button functions
    if (state_sd == SHORT) {
      step_index -=1;
      if (step_index < 0 ) 
      {
        step_index = 5;
      }
      disp_step(fq);
    }
    if (state_sd == FIN_WAIT) {
      eeprom_update_dword((uint32_t *)EE_SAVED_FQ, fq);
      lcd.setCursor(0, 2);
      lcd.print("              ");
      lcd.setCursor(0, 2);
      lcd.print("SAVED");
      delay(500);
      disp_sideband();
    }

  fsm_en();  //call fsm for enter button functions
    if (state_en == SHORT) {   //encoder button pressed, toggle backlight
      if (rit_state == HIGH) {
        rit_state = LOW;
        //Serial.println("RIT OFF");
        disp_rit();
        set_frequency(0);
      }
      else {
        rit_state = HIGH;
        //Serial.println("RIT ON");
        disp_rit();
        set_frequency(0);
      }
        
    }
    if (state_en == LONG) {
      xmit_on();
    }
    if (state_en == FIN) {
      xmit_off();
    }
    
    measure_volts();
}
