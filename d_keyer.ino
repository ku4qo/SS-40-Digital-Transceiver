void update_PaddleLatch()
{
    if (digitalRead(DIT_KEY) == LOW) {
        keyerControl |= DIT_L;
    }
    if (digitalRead(DAH_KEY) == LOW) {
        keyerControl |= DAH_L;
    }
}

void loadWPM (int wpm)
{
    ditTime = 1200/wpm;
}

void check_keyer()
{
  //keyer control section
  switch (keyerState) {
    case IDLE:
        // Wait for direct or latched paddle press
        if ((digitalRead(DIT_KEY) == LOW) ||
                (digitalRead(DAH_KEY) == LOW) ||
                    (keyerControl & 0x03)) {
            update_PaddleLatch();
            keyerState = CHK_DIT;
        }
        break;
 
    case CHK_DIT:
        // See if the dit paddle was pressed
        if (keyerControl & DIT_L) {
            keyerControl |= DIT_PROC;
            ktimer = ditTime;
            keyerState = KEYED_PREP;
        }
        else {
            keyerState = CHK_DAH;
        }
        break;
 
    case CHK_DAH:
        // See if dah paddle was pressed
        if (keyerControl & DAH_L) {
            ktimer = ditTime*3;
            keyerState = KEYED_PREP;
        }
        else {
            keyerState = IDLE;
        }
        break;
 
    case KEYED_PREP:
        // Assert key down, start timing, state shared for dit or dah
        xmit_on();
        ktimer += millis();                 // set ktimer to interval end time
        keyerControl &= ~(DIT_L + DAH_L);   // clear both paddle latch bits
        keyerState = KEYED;                 // next state
        break;
 
    case KEYED:
        // Wait for timer to expire
        if (millis() > ktimer) {          // are we at end of key down ?
        xmit_off();        
          ktimer = millis() + ditTime;    // inter-element time
          keyerState = INTER_ELEMENT;     // next state
        }
        else if (keyerControl & IAMBICB) {
            update_PaddleLatch();           // early paddle latch in Iambic B mode
        }
        break;          
  
    case INTER_ELEMENT:
        // Insert time between dits/dahs
        update_PaddleLatch();               // latch paddle state
        if (millis() > ktimer) {            // are we at end of inter-space ?
            if (keyerControl & DIT_PROC) {             // was it a dit or dah ?
                keyerControl &= ~(DIT_L + DIT_PROC);   // clear two bits
                keyerState = CHK_DAH;                  // dit done, check for dah
            }
            else {
                keyerControl &= ~(DAH_L);              // clear dah latch
                keyerState = IDLE;                     // go idle
            }
        }
        break;
    }
}

void adj_keyer()
{
  lcd.clear();
  lcd.setCursor(0,0);         //set cursor to column 0, row 1
  lcd.print("Adj. Keyer spd");
  lcd.setCursor(0,1);
  lcd.print("with encoder");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,2);
  if (straight_key == false ) {
    sprintf(LCDstr, "%2d", wpm);    //create string to print
    lcd.print(LCDstr);            //display wpm on line 3
  }
  else {
    lcd.print("Straight Key");
  }
  
  while (digitalRead(ENCODER_BTN)) {    //loop until encoder is pressed again
    if(enc_up == 1) {
      if (straight_key == true) { 
        wpm = 5;
        straight_key=false;
      }
      else {
        wpm +=1;
      }
      if (wpm > 35) wpm = 35;
      lcd.setCursor(0,2);
      lcd.print("                    ");
      lcd.setCursor(0,2);
      sprintf(LCDstr, "%2d", wpm);    //create string to print
      lcd.print(LCDstr);            //display wpm on line 3
      enc_up = 0;
    }
    if(enc_dn == 1) {
      wpm -=1;
      if (wpm < 5) {
        wpm = 4;
        straight_key=true;
      }
      lcd.setCursor(0,2);
      lcd.print("                    ");
      lcd.setCursor(0,2);
      if (straight_key == false ) {
        sprintf(LCDstr, "%2d", wpm);    //create string to print
        lcd.print(LCDstr);            //display wpm on line 3
      }
      else {
        lcd.print("Straight Key");
      }
      enc_dn = 0;
    }
  }
  delay(100);     //encoder button pressed to exit menu. debounce
  lcd.clear();    // clear LCD and re-display frequency for operation
  disp_freq(fq); 
  disp_step(fq);
  disp_rit(); 
  loadWPM(wpm);                 // set keyer speed
  EEPROM.update(EE_KEYER_SPEED, wpm); //save new speed in EEPROM
}

void check_sk(void)
{
  if (digitalRead(DIT_KEY) == LOW || digitalRead(DAH_KEY) == LOW) {
    if (sk_on == false) {
      xmit_on();
      sk_on = true;
    }
  }
  else {
    if (sk_on == true) {
      xmit_off();
      sk_on = false;
    }
  }
}
