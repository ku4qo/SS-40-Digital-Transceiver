void calib(void)              //System calibration function, Si5351 clock and BFO
{
  long cal_fq = 10000000; 
  long cal_orig = cal_ofst;   //save existing cal value
  lcd.clear();
  lcd.setCursor(0,0);         //set cursor to column 0, row 1
  lcd.print("ADJUST CLK0 TO");
  lcd.setCursor(0,1);         //set cursor to row 2
  lcd.print("10.000 MHz");  
  lcd.setCursor(0,2);         //set cursor to row 2
  lcd.print("CHANGE WITH ENCODER");  
  lcd.setCursor(0,3);         //set cursor to row 3
  lcd.print("PRESS ENTER TO CONT.");
  wait_key();                   //wait for "Enter" key press, then continue
  lcd.clear();
  disp_freq(cal_fq);
  disp_step(cal_fq);
  Si.setFreq(0, cal_fq);
  while ( digitalRead(ENTER_BTN) ) {      //enter_btn is "true" until pushed

    if(enc_up == 1) {
      cal_fq = cal_fq + step_array[step_index];
      Si.setFreq(0, cal_fq);
      disp_freq(cal_fq);
      enc_up=0;
    }
    else if (enc_dn == 1) {
      cal_fq = cal_fq + (-1 * step_array[step_index]);
      Si.setFreq(0, cal_fq);
      disp_freq(cal_fq);
      enc_dn=0;
    }
    check_step_change(cal_fq);    // check if step size buttons pressed
  }
  
 cal_ofst = 10000000 - cal_fq; //find difference from 10MHz reference
 cal_ofst *= 2.5;                //calculate 25MHz crystal offset value
 cal_ofst += cal_orig;           //factor back in original offset saved in eeprom
 disp_freq(cal_ofst);            //display ofset value
 delay(100);
 wait_key();

 eeprom_write_dword((uint32_t *)EE_SAVED_CAL, cal_ofst); //save new cal in eeprom
 Si.correction(cal_ofst);       //set new calibration
 if ( usb == true ) {          // calculate vfo
      vfo = if_freq + fq;
    }
    else {
      vfo = if_freq - fq;
    }
 Si.setFreq(0, vfo);            //call lib to set vfo frequency 
 Si.enable(0);                  //turn on vfo 

//Si5351 calibration complete; Begin IF Passband calibration
 
  lcd.setCursor(0,0);         //set cursor to column 0, row 0
  lcd.print("ADJUST IF FREQUENCY");
  lcd.setCursor(0,1);         //set cursor to row 1
  lcd.print("TO MATCH IF FILTER");  
  lcd.setCursor(0,2);         //set cursor to row 2
  lcd.print("CENTER @ 700Hz");
  lcd.setCursor(0,3);         //set cursor to row 3
  lcd.print("PRESS ENTER TO CONT.");
  wait_key();                 //wait for ENTER_BTN key press then continue
  lcd.clear();                // clear screen and display if freq and step pointer
  disp_freq(if_freq);
  disp_step(if_freq);

  while ( digitalRead(ENTER_BTN) ) {

    if(enc_up == 1) {
      if_freq += step_array[step_index];
      disp_freq(if_freq);
      enc_up=0;
    }
    else if (enc_dn == 1) {
      if_freq += (-1 * step_array[step_index]);
      disp_freq(if_freq);
      enc_dn=0;
    }
    check_step_change(if_freq);
  }
  
  eeprom_write_dword((uint32_t *)EE_SAVED_IF, if_freq); //save new if frequency cal in eeprom

  disp_freq(fq);                  //display the initial startup frequency, 7,030.000 KHz
  disp_step(fq);                  //display inital step indicator
  disp_rit();
  disp_sideband();
}

void wait_key(void)
{
  while ( digitalRead(ENTER_BTN) ) {}
  delay(100);
  while ( !digitalRead(ENTER_BTN) ) {}
  delay(100);
}

void check_step_change(long fq2)
{
    if ( !digitalRead(UP_BTN) )
    {
      step_index+=1;
      if (step_index > 5 ) 
      {
       step_index = 0;
      }
      disp_step(fq2);
      while ( !digitalRead(UP_BTN)) {} //debounce
      delay(100);
    }

    if ( !digitalRead(DOWN_BTN) )
    {
     step_index -=1;
     if (step_index < 0 ) 
    {
     step_index = 5;
    }
    disp_step(fq2);
      while ( !digitalRead(DOWN_BTN)) {} //debounce
      delay(100);
    }
}
