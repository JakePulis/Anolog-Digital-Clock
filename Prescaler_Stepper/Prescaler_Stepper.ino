

void setPrescaler(int timer, int divisor) {
  byte mode;
  if (timer == 0 || timer == 1) {
    switch (divisor) {
    case 1: mode = 0x01; break;
    case 8: mode = 0x02; break;
    case 64: mode = 0x03; break;
    case 256: mode = 0x04; break;
    case 1024: mode = 0x05; break;
    default: return;
    }
    if (timer == 0) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    }
    else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  }
  else if (timer == 2) {
    switch (divisor) {
    case 1: mode = 0x01; break;
    case 8: mode = 0x02; break;
    case 32: mode = 0x03; break;
    case 64: mode = 0x04; break;
    case 128: mode = 0x05; break;
    case 256: mode = 0x06; break;
    case 1024: mode = 0x07; break;
    default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}


// ------ DEFINITION OF STEP UP AND DOWN ---------
void stepper1_fw() {motor1.stepUp();}
void stepper1_bw() {motor1.stepDown();}
void stepper2_fw() {motor2.stepUp();}
void stepper2_bw() {motor2.stepDown();}
