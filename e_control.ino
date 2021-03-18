void set_frequency(short dir)
{
  fq=fq+(dir * step_array[step_index]);   //change freq by step size
  calc_vfo();
  if ( rit_state == HIGH ) { vfo += rit_freq; }
  Si.setFreq(0, vfo);
  Si.enable(0);
}

void set_rit(short dir)
{
  rit_freq=rit_freq+(dir * step_array[step_index]);
}

void disp_freq(long freq)
{
  long fq_m=freq / 1000000;   // parse frequency to display into chunks
  long rm = freq % 1000000;
  long fq_k=rm / 1000;
  long fq_h = rm % 1000;
  lcd.setCursor(0, 0);    //Clear line
  lcd.print("                    ");
  sprintf(LCDstr, "%.1ld,%.3ld.%.3ld KHz", fq_m, fq_k, fq_h );  //create string with proper formatting
  lcd.setCursor(0, 0);        // set the cursor to column 5, line 0
  lcd.print(LCDstr);
}

void disp_step(long freq)
{
  int ofst = 0;
  ofst=step_index * (-1) + 8; // calculate offset for step indicator
  if (freq >= 10000000) ofst = ofst + 1;
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(ofst, 1);
  lcd.print("^");         //Erase old caret, write a new caret
}

void disp_keyer(void)
{
  
}

void disp_rit(void)
{
  lcd.setCursor(0, 3);
  lcd.print("              ");
  lcd.setCursor(0, 3);
  if (rit_state == HIGH)
  {
    lcd.print("RIT");
    lcd.setCursor(4, 3);
    lcd.print(rit_freq);
  }
}

void disp_sideband(void)
{
  lcd.setCursor(0, 2);
  lcd.print("              ");
  lcd.setCursor(0, 2);
  if (usb == true)
  {
    lcd.print("USB");
  }
  else {
    lcd.print("LSB");
  }
}

void xmit_on(void)  //suggestion, only run setFreq if fq has changed. 
{
      digitalWrite(RX_MUTE, LOW);           //mute receiver audio
      delay(5);                             //wait for audio to quit
      digitalWrite(RX_PIN, LOW);            //turn off Rx PIN diode switch
      delay(2);                             //wait for transients to die out   
      digitalWrite(TX_PIN, HIGH);           //turn on Tx PIN diode switch
      delay(2);                             //wait for transients to die out
      if ( rit_state == HIGH ) {            //vfo for correct sidetone during transmit
        calc_vfo();
        Si.setFreq(0, vfo);             
      }
      Si.setFreq(2, fq);                    //set transmit frequency
      Si.enable(2);                         //turn on xmit frequency
      digitalWrite(XMIT_EN, HIGH);          //enable CLK2 drive to finals
      digitalWrite(XMIT_PWR, HIGH);         //turn on power to final amplifier and arduino L LED
}

void xmit_off(void)
{
        digitalWrite(XMIT_PWR, LOW);        //turn off power to final amplifier
        delay(5);                           //wait to turn off xmit to allow cw envelope to decay and avoid keyclicks
        digitalWrite(XMIT_EN, LOW);          //disable CLK2 drive to finals
        Si.disable(2);                      //turn off xmit frequency
        digitalWrite(TX_PIN, LOW);          //turn off Tx PIN diode switch
        delay(2);                           //wait for transients to die out
        if ( rit_state == HIGH ) {          //recover correct vfo for RIT post transmit
          set_frequency(0);
        }
        digitalWrite(RX_PIN, HIGH);         //turn on Rx PIN diode switch
        delay(2);                           //wait for transients to die out             
        digitalWrite(RX_MUTE, HIGH);        //unmute receiver audio 
}

void measure_volts(void)
{
  if (millis() - v_time > 1000) {
    vbatt = 3 * analogRead(PS_MONITOR) * vpb;
    lcd.setCursor(14,3);
    sprintf(LCDstr, "%02d.%02dV", (int)vbatt, (int)(vbatt*100)%100);    //create string to print
    lcd.print(LCDstr);            //display battery voltage on line 4 in right corner
    v_time = millis();            //save time of last display
  }
}

void calc_vfo(void)
{
  if ( usb == true ) {          // calculate vfo
    vfo = if_freq + fq;
  }
  else {
    vfo = if_freq - fq;
  }
}
