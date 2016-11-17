#ifdef CALIBRATE
void CalibrateCenter()
{
  Orion.tone(NOTE_C6,100);
  
  //*************
  //clear offsets
  //*************
  for(int i = 0;i<29;i++){
    if(IKPins[i]!=-1){
      Orion.setAOffset(IKPins[i],0);
    }
  }
  TailPitchServo.setAOffset(0);
  TailYawServo.setAOffset(0);
  
  //**************************************************
  //Wait for user to move servos to calibration points
  //**************************************************
  while(!ps2x.buttonPressed(PSB_START) && !BTNA){
    if(ps2x.buttonPressed(PSB_SELECT)){
      Orion.tone(NOTE_G6,100);
      return;
    }
    Orion.checkLipo();

#ifdef DEBUG
    Serial.write((unsigned char)0);    
    for(int i = 0;i<29;i++){
      if(IKPins[i]!=-1){
        int offset = Orion.queryFBAngle(IKPins[i]);
        Serial.print(IKPins[i]);
        Serial.print(":");
        Serial.println(offset);
      }
    }
    int offset = TailPitchServo.queryFBAngle();
    Serial.print("A10");
    Serial.print(":");
    Serial.println(offset);
    offset = TailYawServo.queryFBAngle();
    Serial.print("A9");
    Serial.print(":");
    Serial.println(offset);
#endif //DEBUG

    delay(100);
  }
  while(BTNA);
  
  //****************
  //Save new offsets
  //****************
  for(int i = 0;i<29;i++){
    if(IKPins[i]!=-1){
      Orion.setAOffset(IKPins[i],Orion.queryFBAngle(IKPins[i])+ServoAOffsets[i]);
    }
  }
  TailPitchServo.setAOffset(TailPitchServo.queryFBAngle());
  TailYawServo.setAOffset(TailYawServo.queryFBAngle());

  Orion.tone(NOTE_B6,100);
}

void CalibrateRange()
{
  Orion.tone(NOTE_D6,100);

  int servomin[29];
  int servomax[29];
  for(int i = 0;i<29;i++){
    if(IKPins[i]!=-1){
      servomin[i] = 3000;
      servomax[i] = -3000;
    }
  }

  while(!ps2x.buttonPressed(PSB_START) && !BTNB){
    if(ps2x.buttonPressed(PSB_SELECT)){
      Orion.tone(NOTE_G6,100);
      return;
    }
    Orion.checkLipo();

#ifdef DEBUG
    Serial.write(uint8_t(0));
#endif
    for(int i=0;i<29;i++){
      if(IKPins[i]!=-1){
        int angle = Orion.queryFBAngle(IKPins[i]);
        if(servomin[i]>angle && angle<2300)
          servomin[i]=angle;
        if(servomax[i]<angle && angle>-2300)
          servomax[i]=angle;
#ifdef DEBUG
        Serial.print(servomin[i]);
        Serial.print(" ");
        Serial.println(servomax[i]);
#endif
      }
    }    
    delay(100);
  }
  while(BTNB);

  for(int i = 0;i<29;i++){
    if(IKPins[i]!=-1){
      Orion.setServoMin(IKPins[i],servomin[i]);
      Orion.setServoMax(IKPins[i],servomax[i]);
    }
  }

  Orion.tone(NOTE_B6,100);
}

void Calibrate()
{
  Orion.red(true);

  for(int i=0;i<24;i++){
    Orion.setServoDegree(IKPins[i],DEGREE);
    Orion.setServoDir(IKPins[i],reverse[i]);
    Orion.setServoIKPin(i/4,i%4,IKPins[i]);
    Orion.setServoIKLength(i/4,i%4,IKLengths[i]);
  }    
 
  while(!ps2x.buttonPressed(PSB_RED)){
    if(ps2x.buttonPressed(PSB_PINK))
      asm volatile ("jmp 0x0000");  //reset
    if(ps2x.buttonPressed(PSB_GREEN))
      CalibrateCenter();
    if(ps2x.buttonPressed(PSB_BLUE))
      CalibrateRange();  
    if(BTNB){
      while(BTNB);
      CalibrateCenter();
      break;
    }
    if(BTNA){
      while(BTNA);
      CalibrateRange();
      break;
    }
  }

  for(int i = 0;i<6;i++){
      Orion.setServoIKDir(i,0);
  }
  Orion.enableIKMinMax(false);
  Orion.writeRegisters();  

  int offset = TailPitchServo.getAOffset();
  EEPROM.write((29*2)+0,offset>>8);
  EEPROM.write((29*2)+1,offset&0xFF);
  offset = TailYawServo.getAOffset();
  EEPROM.write((30*2)+0,offset>>8);
  EEPROM.write((30*2)+1,offset&0xFF);
  EEPROM.write(510,EEKEY>>8);
  EEPROM.write(511,EEKEY);

  delay(250);

  Orion.red(false);

  asm volatile ("jmp 0x0000");  //reset
}
#endif //CALIBRATE
