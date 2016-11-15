void StopServos()
{
  for(int i = 0;i<29;i++)
    if(IKPins[i]!=-1)
      Orion.stopPulse(IKPins[i]);
  //TailPitchServo.stopPulse();
  //TailYawServo.stopPulse();
}


//=======================================================================================================
// QueryServoFBAngle
//=======================================================================================================
int QueryServoFBAngle(int Pin) {
  for(Pin=0;Pin<24;Pin++)
  {
  if (Pin >= 0)
    return Orion.queryFBAngle(Pin);
  return 0;
  }    
}

//=======================================================================================================
// PrintServoInfo
//=======================================================================================================
void PrintServoInfo() {
  // Will print out information about each of the servos...
  int i;
  for (int j=0; j < NUM_SERVOS; j++) {
    Serial.print("\n");
    Serial.print(i = servoPins[j], DEC);
    Serial.print("(");
    Serial.print(servoPinsNames[j]);
    Serial.print(")");
    Serial.print(" FBangle:");
    Serial.print(Orion.queryAngle(i), DEC);
   
   //Serial.print(Orion.queryFBAngle (HeadRollPin));
    if ( i >= 0) {
  
      Serial.print("  getXPos :");
      Serial.print(Orion.getXPos(j), DEC);
      Serial.print(" getYPos ");
      Serial.print(Orion.getYPos(j), DEC);
      Serial.print(" getZPos" );
      Serial.print(Orion.getZPos(j),DEC);
      Serial.print(" getWPos:");
      Serial.print(Orion.getWPos(j), DEC);
      /*Serial.print(" W:");
      Serial.print(Orion.getWPos(i), DEC);*/
   }
  
  }
}  
//=======================================================================================================
// Initialisation
//=======================================================================================================
void initangles(){
  for (int j=0; j < 29; j++) {
  //Orion.setTime( 5000 );
  //Orion.setAngle(servoPins[j], 0 );
  //Orion.setServoMin( servoPins[j], -900 );
  //Orion.setServoMax( servoPins[j], 900 );
   Serial.print (Orion.queryAOffset(IKPins[j]));
    Serial.print("\n");
  //Orion.execute();
  }
}


