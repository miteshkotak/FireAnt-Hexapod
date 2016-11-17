void loop() {
  if(Orion.checkLipo()){
    StopServos();
    return; //Battery too low to do anything.
  }

  TailPitchServo.update();
  TailYawServo.update();

#ifdef CALIBRATE
  if(ps2x.buttonPressed(PSB_SELECT) || BTNB){
    while(BTNB);
    onoff=false;
    Orion.tone(NOTE_A6,100);
    StopServos();
    Calibrate();
  }
#endif //CALIBRATE

  boolean btna = BTNA;
  if(ps2x.buttonPressed(PSB_START) || btna){
    while(BTNA);
#ifdef DEMO
    if(btna)
      demomode=true;
#endif
    if(onoff){
      onoff=false;
      Orion.tone(NOTE_G4,100);
      StopServos();
    }
    else{
      onoff=true;

      //Wake up and position legs for standing.
      Orion.setTime(0);
      
      for(int i=0;i<29;i++){
        if(IKPins[i]!=-1)
          Orion.setAngle(IKPins[i],Orion.queryFBAngle(IKPins[i]));
      }
      Orion.execute();

      int angle = TailPitchServo.queryFBAngle();
      TailPitchServo.setAngle(angle,20);
      angle = TailYawServo.queryFBAngle();
      TailYawServo.setAngle(angle,20);
      delay(100);

      InitState();
      
      Orion.setTime(1000);

      ProcessAnt();
      ProcessLegs();
 
      for(int i = 0;i<50;i++){
        TailPitchServo.update();
        TailYawServo.update();
        delay(20);
      }
      Orion.setTime(100);

      Orion.tone(NOTE_A5,100);
      Orion.tone(NOTE_B5,100);
      Orion.tone(NOTE_C6,100);

#ifdef PS2
      ps2x.newButtonState();  //Clears pressed/released states
#endif

#ifdef DEMO
      if(demomode)
        Demo();
#endif
    }
  }

  if(!onoff){
    Orion.green(true);
    return;  
  }
 
#ifdef PS2
  //Heart beat LED
  long time = millis() - lastbeat;
  if(time>200){
    lastbeat=millis();
    heartbeat^=heartbeat;
    Orion.green(heartbeat);
  }

  if(ps2x.buttonPressed(PSB_L1)){
    Orion.tone(NOTE_G6,100);
  }
  if(ps2x.buttonReleased(PSB_L1)){
    Orion.tone(NOTE_G5,100);
  }

  if(ps2x.buttonPressed(PSB_GREEN)){
    Rate+=1;
    if(Rate>15)
      Rate=15;
    SetForce();
    Orion.tone(NOTE_A5+(400+(Rate*25)),100);
  }

  if(ps2x.buttonPressed(PSB_BLUE)){
    Rate-=1;
    if(Rate<1)
      Rate=1;
    SetForce();
    Orion.tone(NOTE_A5+(400+(Rate*25)),100);
  }

  if(ps2x.buttonPressed(PSB_PINK)){
    if(Gait<9)
      Gait++;
    Orion.gaitSelect((OrionClass::GAITTYPE)Gait);
    Orion.tone(NOTE_G4+(Gait*100),100);
  }
  if(ps2x.buttonPressed(PSB_RED)){
    if(Gait>0)
      Gait--;
    Orion.gaitSelect((OrionClass::GAITTYPE)Gait);
    Orion.tone(NOTE_G4+(Gait*100),100);
  }
  
  if(!ps2x.button(PSB_L1)){
    if(ps2x.buttonPressed(PSB_PAD_UP)){
      Ydist+=(MAXY/16);
      if(Ydist>MAXY/2)
        Ydist=MAXY/2;
      Orion.tone(Ydist+400,100);
    }
    if(ps2x.buttonPressed(PSB_PAD_DOWN)){
      Ydist-=(MAXY/16);
      if(Ydist<(MAXY/16))
        Ydist=(MAXY/16);
      Orion.tone(Ydist+400,100);
    }
  }

  if(ps2x.buttonPressed(PSB_L3)){
    if(ps2x.button(PSB_L1)){
      Orion.setBalanceMode((Orion.getBalanceMode()+1)&0x3);
      Orion.tone(NOTE_E5+(100*Orion.getBalanceMode()),100);
    }
    else{
      RotateMode++;
      if(RotateMode>3)
        RotateMode=0;
      Orion.tone(NOTE_D5+(100*RotateMode),100);
    }
  }

  if(ps2x.button(PSB_L1)){
    if(ps2x.buttonPressed(PSB_R3)){
      ReturnToCenter^=0x1;
      if(ReturnToCenter&0x1)
        Orion.tone(NOTE_A5,100);
      else
        Orion.tone(NOTE_B5,100);
    }
  }
  else{
    if(ps2x.buttonPressed(PSB_R3)){
      RightJoystickMode^=0x1;
      if(RightJoystickMode&0x1)
        Orion.tone(NOTE_A5,100);
      else
        Orion.tone(NOTE_B5,100);
    }
  }

  //If ps2 values have been updated
  //process analog controls
  if(ps2x.isUpdated()){
#ifdef FILTER
    int RX = filter_update(rxfilter,-ps2x.analog(PSS_RX),1);
    int RY = filter_update(ryfilter,-ps2x.analog(PSS_RY),1);
    int LX = filter_update(lxfilter,-ps2x.analog(PSS_LX),1);
    int LY = filter_update(lyfilter,-ps2x.analog(PSS_LY),1);
    int DX = filter_update(dxfilter,-ps2x.analog(PSAB_PAD_LEFTRIGHT),1);
    int DY = filter_update(dyfilter,ps2x.analog(PSAB_PAD_UPDOWN),1);
#else
    int RX = -ps2x.analog(PSS_RX);
    int RY = -ps2x.analog(PSS_RY);
    int LX = -ps2x.analog(PSS_LX);
    int LY = -ps2x.analog(PSS_LY);
    int DX = -ps2x.analog(PSAB_PAD_LEFT);
    int DY = ps2x.analog(PSAB_PAD_UP);
#endif

    RX=ScaleDeadzone(RX,127,10);
    RY=ScaleDeadzone(RY,127,10);
    LX=ScaleDeadzone(LX,127,10);
    LY=ScaleDeadzone(LY,127,10);
    DX=ScaleDeadzone(DX,255,40);
    DY=ScaleDeadzone(DY,255,40);

    if(ps2x.button(PSB_L2)){
      OpenPincers(ps2x.analog(PSAB_L2)/(16-Rate),450);
    }
    if(ps2x.button(PSB_R2)){
      ClosePincers(ps2x.analog(PSAB_R2)/(16-Rate),0);
    }

    if(RotateMode==3){
      //Head Control Mode
      CalcHeadPitch(RY/(16-Rate),450);
      CalcHeadYaw(RX/(16-Rate),450);
      CalcHeadRoll(LX/(16-Rate),900);
    }
    else{
      //Body translation and rotations calculations
      if(RotateMode)
        CalcBodyRoll(DX/(16-Rate),MAXBODYROLL);
      else
        CalcBodyYaw(DX/(16-Rate),MAXBODYYAW);

      if(ps2x.button(PSB_L1)){
        CalcBodyOffsetY(-DY/(16-Rate),MAXY);
        CalcBodyOffsetX(4*LX/(16-Rate),MAXX*3/2);
        CalcBodyOffsetZ(4*LY/(16-Rate),MAXZ*2);
      }    
      else{
        if(RotateMode==1){
          CalcBodyYaw(LX/(16-Rate),MAXBODYYAW);
          CalcBodyPitch(LY/(16-Rate),MAXBODYPITCH);
        }
        else
        if(RotateMode==2){
          CalcBodyYaw(LX/(16-Rate),MAXBODYYAW);
          CalcBodyPitch(LY/(16-Rate),MAXBODYPITCH);
        }
        else{
          CalcBodyRoll(LX/(16-Rate),MAXBODYROLL);
          CalcBodyPitch(LY/(16-Rate),MAXBODYPITCH);
        }
      }

      if(RightJoystickMode){
        CalcBodyOffsetX(4*RX/(16-Rate),MAXX*3/2);
        CalcBodyOffsetZ(4*RY/(16-Rate),MAXZ*2);
      }
      else{
        Xdist = (long)RY*MAXX/128;
        if(ps2x.button(PSB_L1))
          Zdist = (long)RX*MAXZ/128;
        else{
          Rdist = -(long)RX*MAXR/128;
        }
      }
    }
    
    //body position reset handlers
    if(ps2x.button(PSB_L1)){
      if(ps2x.button(PSB_R1)){
        ResetBodyOffsetY();
        ResetBodyOffsetX();
        ResetBodyOffsetZ();
      }
    }
    else{
      if(ps2x.button(PSB_R1)){
        ResetBodyYaw();
        ResetBodyRoll();
        ResetBodyPitch();
        ResetHeadYaw();
        ResetHeadRoll();
        ResetHeadPitch();
      }
    }

    if(ReturnToCenter){
      if(DX==0){
        ResetBodyYaw();
      }
      if(LX==0){
        if(!RightJoystickMode)
          ResetBodyOffsetX();
        ResetBodyRoll();
      }
      if(RX==0 && RightJoystickMode){
        ResetBodyOffsetX();
      }
      if(LY==0){
        if(!RightJoystickMode)
          ResetBodyOffsetZ();
        ResetBodyPitch();
      }
      if(RY==0 && RightJoystickMode){
        ResetBodyOffsetZ();
      }
    }

    ProcessAnt();
    ProcessLegs();
  }
#endif  //PS2
}

