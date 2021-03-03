

void PinA(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if(reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    //encoderPos --; //decrement the encoder's position count
    enc_up=1;
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void PinB(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    //encoderPos ++; //increment the encoder's position count
    enc_dn=1;
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void check_encoder(){
    if(enc_up == 1) {
      if (rit_state == LOW) {
        set_frequency(1);
        disp_freq(fq);
      }
      else {
        set_rit(1);
        disp_rit();
        set_frequency(0);
      }
      enc_up=0;
    }
    else if (enc_dn == 1) {
      if (rit_state == LOW) {
      set_frequency(-1);
      disp_freq(fq);
      }
      else {
        set_rit(-1);
        disp_rit();
        set_frequency(0);
      }
      enc_dn=0;
    }
}
