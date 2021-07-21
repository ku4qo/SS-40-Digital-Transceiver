void setup(void) {
  //Serial.begin(115200);
  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  attachInterrupt(0,PinA,RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1,PinB,RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)

  Wire.begin();     //initialize wire functionality
  lcd.init();       //initialize lcd
  lcd.backlight();  //turn on backlight
 
  fq = eeprom_read_dword((const uint32_t *)EE_SAVED_FQ); //read saved freq from eeprom. Reset value if it is uninitialized or unreasonable
  if (fq < 7000000 | fq > 7300000) {
    fq=7030000;
    eeprom_write_dword((uint32_t *)EE_SAVED_FQ, fq);
  }
  cal_ofst = eeprom_read_dword((const uint32_t *)EE_SAVED_CAL);  //read saved calibration offset from eeprom. Reset value if it is uninitialized or unreasonable.
  if (cal_ofst < -256000 | cal_ofst > 256000) {
    cal_ofst=0;
    eeprom_write_dword((uint32_t *)EE_SAVED_CAL, cal_ofst);
  }
  
  if_freq = eeprom_read_dword((const uint32_t *)EE_SAVED_IF);  //read saved IF frequency from eeprom
    if (if_freq < 10999500 | if_freq > 11000500) {
    if_freq = if_guess;
    eeprom_write_dword((uint32_t *)EE_SAVED_IF, if_freq);
  }

  wpm = eeprom_read_dword((const uint32_t *)EE_KEYER_SPEED); //read saved keyer speed
    if (wpm < 0 | wpm > 35) {
      wpm = 10;                                                //set wpm to 10 if unreasonable
      eeprom_write_dword((uint32_t *)EE_KEYER_SPEED, wpm);
    }
    if (wpm == 4) { straight_key = true;}
    
    pinMode(ENCODER_BTN, INPUT_PULLUP);
    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(DOWN_BTN, INPUT_PULLUP);
    pinMode(ENTER_BTN, INPUT_PULLUP);
    pinMode(DIT_KEY, INPUT_PULLUP);
    pinMode(DAH_KEY, INPUT_PULLUP);

    pinMode(TX_PIN, OUTPUT);      //Tx PIN switch control
    digitalWrite(TX_PIN, LOW);    //Initialize low 
    pinMode(RX_PIN, OUTPUT);      //Rx PIN switch control
    digitalWrite(RX_PIN, HIGH);   //Initialize high
    pinMode(RX_MUTE,  OUTPUT);    //Rx mute line
    digitalWrite(RX_MUTE, HIGH);  //Initialized high
    pinMode(XMIT_PWR, OUTPUT);    //this output applies power to the finals for transmitting and shapes the CW wave
    digitalWrite(XMIT_PWR, LOW);  //turn off power to finals
    pinMode(XMIT_EN, OUTPUT);     //gates CLK2 to finals
    digitalWrite(XMIT_EN, LOW);   //disable CLK2 drive to finals
 
    Si.init(25000000L);             //si5351 crystal freq 25MHz
    Si.correction(cal_ofst);        // correct si5351 crystal frequency - calculate for each new si5351 board
 
// Enable the desired outputs with some frequencies
    calc_vfo();                       //calculate vfo freq
    Si.setFreq(0, vfo);                 // CLK0 is VFO
    
// set drive levels
    Si.setPower(0, SIOUT_4mA);
    Si.setPower(2, SIOUT_8mA);

// enable the outputs
    Si.enable(0);
    Si.disable(2);                 //turn off xmit
    lcd.setCursor(3,1);
    lcd.print("SS-40 DIGITAL");
    delay(2000);
    lcd.clear();

    keyerState = IDLE;
    keyerControl = IAMBICB;      // Or 0 for IAMBICA
    loadWPM(wpm);                // set keyer speed

    disp_freq(fq);                  //display the initial startup frequency, 7,030.000 KHz
    disp_step(fq);                  //display inital step indicator
    disp_rit();                     //display initial rit state
    disp_sideband();

    if (!digitalRead(ENTER_BTN) ) { start_cal = true; }   //start calibration if enter is pressed
}
