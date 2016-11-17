void setup() {
#ifdef DEBUG
  Serial.begin(625000);
#endif

  // initialize Orion
  Orion.begin();
  Orion.green(true);
  Orion.red(false);
  
  InitArduinoServos();

  StopServos();

  Orion.setBalanceMode(0x2);
  Orion.setBalanceTransRate(50);
  Orion.setBalanceAngleRate(200);
  
  InitState();

  //Init other system variables
  lastbeat = millis();
  onoff = false;
  gripped = false;
  
/*  Orion.setForce(LFC,3000);
  Orion.setForce(RFC,3000);
  Orion.enableForce(LFC,1);
  Orion.enableForce(RFC,1);*/
  
  Orion.setForce(PincerLPin,1600);
  Orion.setForce(PincerRPin,1600);
  Orion.enableForce(PincerLPin,1);
  Orion.enableForce(PincerRPin,1);
  
  Orion.tone(NOTE_C6,100);
  Orion.tone(NOTE_B5,100);
  Orion.tone(NOTE_A5,100);

#ifdef DEMO
  demomode = false;
  walktotal = 0;
#endif

  //Clears pressed/released states
  ps2x.newButtonState();
  delay(100);
  ps2x.newButtonState();
}

