void fsm_eb() {
  switch (state_eb) {
    case RST:
      state_eb = WAIT;
    break;

    case WAIT:
      if (digitalRead(ENCODER_BTN) == LOW) {state_eb = ARM;}
    break;

    case ARM:
      t_eb = millis();
      state_eb = DEBOUNCE;
    break;

    case DEBOUNCE:
      if (millis() - t_eb > bounce_delay) {state_eb = LIFT;}  //if debounce timer expires - good press
      if (digitalRead(ENCODER_BTN) == HIGH) {state_eb = RST;} //bounce, go back to reset state
    break;

    case LIFT:
      if (digitalRead(ENCODER_BTN) == HIGH) {state_eb = SHORT;}
      if (millis() - t_eb > hold_delay) {state_eb = LONG;}
    break;

    case SHORT:
      state_eb = RST;
    break;

    case LONG:
      state_eb = RELEASE;
    break;

    case RELEASE:
      if (digitalRead(ENCODER_BTN) == HIGH) {state_eb = FIN_WAIT;}
    break;

    case FIN_WAIT:
      state_eb = RST;
    break;
    
  }
}

void fsm_su() {
  switch (state_su) {
    case RST:
      state_su = WAIT;
    break;

    case WAIT:
      if (digitalRead(UP_BTN) == LOW) {state_su = ARM;}
    break;

    case ARM:
      t_su = millis();
      state_su = DEBOUNCE;
    break;

    case DEBOUNCE:
      if (millis() - t_su > bounce_delay) {state_su = LIFT;}  //if debounce timer expires - good press
      if (digitalRead(UP_BTN) == HIGH) {state_su = RST;} //bounce, go back to reset state
    break;

    case LIFT:
      if (digitalRead(UP_BTN) == HIGH) {state_su = SHORT;}
      if (millis() - t_su > hold_delay) {state_su = LONG;}
    break;

    case SHORT:
      state_su = RST;
    break;

    case LONG:
      state_su = RELEASE;
    break;

    case RELEASE:
      if (digitalRead(UP_BTN) == HIGH) {state_su = FIN_WAIT;}
    break;

    case FIN_WAIT:
      state_su = RST;
    break;
  }
}

void fsm_sd() {
    switch (state_sd) {
    case RST:
      state_sd = WAIT;
    break;

    case WAIT:
      if (digitalRead(DOWN_BTN) == LOW) {state_sd = ARM;}
    break;

    case ARM:
      t_sd = millis();
      state_sd = DEBOUNCE;
    break;

    case DEBOUNCE:
      if (millis() - t_sd > bounce_delay) {state_sd = LIFT;}  //if debounce timer expires - good press
      if (digitalRead(DOWN_BTN) == HIGH) {state_sd = RST;} //bounce, go back to reset state
    break;

    case LIFT:
      if (digitalRead(DOWN_BTN) == HIGH) {state_sd = SHORT;}
      if (millis() - t_sd > hold_delay) {state_sd = LONG;}
    break;

    case SHORT:
      state_sd = RST;
    break;

    case LONG:
      state_sd = RELEASE;
    break;

    case RELEASE:
      if (digitalRead(DOWN_BTN) == HIGH) {state_sd = FIN_WAIT;}
    break;

    case FIN_WAIT:
      state_sd = RST;
    break;
  }
}

void fsm_en() {
  switch (state_en) {
    case RST:
      state_en = WAIT;
    break;

    case WAIT:
      if (digitalRead(ENTER_BTN) == LOW) {state_en = ARM;}
    break;

    case ARM:
      t_en = millis();
      state_en = DEBOUNCE;
    break;

    case DEBOUNCE:
      if (millis() - t_en > bounce_delay) {state_en = LIFT;}  //if debounce timer expires - good press
      if (digitalRead(ENTER_BTN) == HIGH) {state_en = RST;} //bounce, go back to reset state
    break;

    case LIFT:
      if (digitalRead(ENTER_BTN) == HIGH) {state_en = SHORT;}
      if (millis() - t_en > hold_delay) {state_en = LONG;}
    break;

    case SHORT:
      state_en = RST;
    break;

    case LONG:
      state_en = RELEASE;
    break;

    case RELEASE:
      if (digitalRead(ENTER_BTN) == HIGH) {state_en = CANCEL;}
    break;

    case CANCEL:
      if (digitalRead(ENTER_BTN) == LOW) {state_en = FIN;}
      if (millis() - t_en > tune_timeout) {state_en = FIN;}
    break;

    case FIN:
      state_en = FIN_WAIT;
    break;

    case FIN_WAIT:
      if (digitalRead(ENTER_BTN) == HIGH) {state_en = RST;}
    break;
  }
}
