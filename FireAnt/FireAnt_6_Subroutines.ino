void InitState()
{
  RightJoystickMode=0;
  ReturnToCenter=0;
  RotateMode=0;
  Rate=11;
  Xdist=0;
  Ydist=MAXY/4;
  Zdist=0;
  Rdist=0;
  BodyOffsetX=0;
  BodyOffsetY=0;
  BodyOffsetZ=0;
  BodyRotOffsetZ=0;
  BodyYaw=0;
  BodyPitch=0;
  BodyRoll=0;
  XLegAdj=0;
  ZLegAdj=0;

  for(int i=0;i<6;i++){
    Orion.setBodyPos(i,DefBodyPosX[i],DefBodyPosY[i],DefBodyPosZ[i]);
    Orion.setLegPos(i,DefLegPosX[i],DefLegPosY[i],DefLegPosZ[i]);
  }
  HeadPitch=-BodyPitch*3/2 + 180;
  HeadYaw = DefAntPos[1];
  HeadRoll = DefAntPos[2];
  PincerL = DefAntPos[3];
  PincerR = DefAntPos[4];
  TailPitch = DefAntPos[5];
  TailYaw = DefAntPos[6];

  HeadPitchOffset = 0;
  HeadYawOffset = 0;
  HeadRollOffset = 0;

  Gait = OrionClass::TRI4;
  Orion.gaitSelect((OrionClass::GAITTYPE)Gait);
}

void SetForce()
{
  int force = 1600;
  switch(Rate){
  case 11:
    force = 1600;
    break;      
  case 12:
    force = 1800;
    break;      
  case 13:
    force = 2000;
    break;      
  case 14:
    force = 2500;
    break;      
  case 15:
    force = 3500;
    break;      
  }
  Orion.setForce(PincerLPin,force);
  Orion.setForce(PincerRPin,force);
}

void ProcessAnt()
{
  //Ant head/tail auto calculations
  if(RotateMode==0)
    HeadPitch=HeadPitchOffset-(BodyPitch*2) + 80;
  else
    HeadPitch=HeadPitchOffset+BodyPitch*3/2;
  HeadRoll=HeadRollOffset+(BodyRoll*3/2)-(Rdist/2);
  TailPitch=BodyPitch*3/2;
  HeadYaw=HeadYawOffset+(-BodyYaw*1.5)-(Rdist/2);
  TailYaw=(-BodyYaw*1.5)-(Rdist/2);

  //Set Ant special servos
  Orion.setAngle(HeadPitchPin,HeadPitch+Orion.getBalancePitch());
  Orion.setAngle(HeadYawPin,HeadYaw);
  Orion.setAngle(HeadRollPin,HeadRoll-Orion.getBalanceRoll());
  if(!gripped && (Orion.queryForceTrig(PincerLPin) || Orion.queryForceTrig(PincerRPin))){
    PincerL = Orion.queryFBAngle(PincerLPin)-55;
    PincerR = Orion.queryFBAngle(PincerRPin)-55;
    gripped=true;
  }
  Orion.setAngle(PincerLPin,PincerL);
  Orion.setAngle(PincerRPin,PincerR);
  TailYawServo.setAngle(TailYaw,100);
  TailPitchServo.setAngle(TailPitch+Orion.getBalancePitch(),100);
}

void ProcessLegs()
{
  //mixing range limiter
  long xratio = ((long)abs(Xdist)<<16)/MAXX;
  long rratio = ((long)abs(Rdist)<<16)/MAXR;
  if((xratio+rratio)>0x10000){
    Xdist = ((long)Xdist<<16)/(xratio+rratio);
    Rdist = ((long)Rdist<<16)/(xratio+rratio);
  }
  
  //Sends new commands to Orion if there are any changes
  Orion.execGait(BodyYaw,
                 BodyRoll,
                 BodyPitch,
                 BodyOffsetX,
                 BodyOffsetY,
                 DefBodyRotOffset[RotateMode],
                 BodyOffsetX,
                 BodyOffsetY,
                 BodyOffsetZ,
                 Xdist,
                 Ydist,
                 Zdist,
                 Rdist,
                 XLegAdj,
                 ZLegAdj,
                 16-Rate,
                 false);
}

boolean calcmove(long rate,int &current,int target)
{
  if(rate<0){
    rate=-rate;
    target=-target;
  }

  if(current<target){
    current+=rate;
    if(current<target)
      return false;
  }
  if(current>target){
    current-=rate;
    if(current>target)
      return false;
  }
  current=target;
  return true;
}

boolean OpenPincers(long val,int target)
{
  gripped=false;
  Orion.enableForce(PincerRPin,0);
  Orion.enableForce(PincerLPin,0);
  return calcmove(val,PincerL,target) & calcmove(val,PincerR,target);
}

boolean ClosePincers(long val,int target)
{
  if(!(Orion.queryForceTrig(PincerLPin) | Orion.queryForceTrig(PincerRPin))){
    Orion.enableForce(PincerRPin,1);
    Orion.enableForce(PincerLPin,1);
    return calcmove(val,PincerL,target) & calcmove(val,PincerR,target);
  }
  
  return true;
}

boolean CalcHeadPitch(long val,int target)
{
  return calcmove(val,HeadPitchOffset,target);
}
boolean CalcHeadYaw(long val,int target)
{
  return calcmove(val,HeadYawOffset,target);
}
boolean CalcHeadRoll(long val,int target)
{
  return calcmove(val,HeadRollOffset,target);
}

boolean CalcBodyYaw(long val,int target)
{
  return calcmove(val,BodyYaw,target);
}

boolean CalcBodyRoll(long val,int target)
{
  return calcmove(val,BodyRoll,target);
}

boolean CalcBodyPitch(long val,int target)
{
  return calcmove(val,BodyPitch,target);
}

boolean CalcBodyOffsetX(long val,int target)
{
  return calcmove(val,BodyOffsetX,target);
}

boolean CalcBodyOffsetY(long val,int target)
{
  return calcmove(val,BodyOffsetY,target);
}

boolean CalcBodyOffsetZ(long val,int target)
{
  return calcmove(val,BodyOffsetZ,target);
}

void ResetBodyOffsetY()
{
  calcmove(64,BodyOffsetY,0);
}

void ResetBodyOffsetX()
{
  calcmove(64,BodyOffsetX,0);
}

void ResetBodyOffsetZ()
{
  calcmove(64,BodyOffsetZ,0);
}

void ResetBodyYaw()
{
  calcmove(20,BodyYaw,0);
}

void ResetBodyRoll()
{
  calcmove(20,BodyRoll,0);
}

void ResetBodyPitch()
{
  calcmove(20,BodyPitch,0);
}

void ResetHeadYaw()
{
  calcmove(20,HeadYawOffset,0);
}

void ResetHeadRoll()
{
  calcmove(20,HeadRollOffset,0);
}

void ResetHeadPitch()
{
  calcmove(20,HeadPitchOffset,0);
}

void StopServos()
{
  for(int i = 0;i<29;i++)
    if(IKPins[i]!=-1)
      Orion.stopPulse(IKPins[i]);
  TailPitchServo.stopPulse();
  TailYawServo.stopPulse();
}

void InitArduinoServos()
{
  TailPitchServo.begin();
  TailYawServo.begin();
  unsigned int key = (int)((EEPROM.read(510)<<8) | EEPROM.read(511));
  if(key==EEKEY){
    int offset=(int)((EEPROM.read((29*2)+0)<<8) | (EEPROM.read((29*2)+1)&0xFF));
    TailPitchServo.setAOffset(offset);
    offset=(int)((EEPROM.read((30*2)+0)<<8) | (EEPROM.read((30*2)+1)&0xFF));
    TailYawServo.setAOffset(offset);
  }
}

int ScaleDeadzone(int val,int range,int deadzone)
{
    if(val<-DEADZONE)
      val+=DEADZONE;
    else
    if(val>DEADZONE)
      val-=DEADZONE;
    else
      val=0;
    val=(long)val*range/(range-deadzone);

    return val;
}
