//=======================================================================================================
// Stop Servos
//=======================================================================================================
void StopServos(void) {
  for(int i = 0;i<18;i++) {
    if (ALL_SERVO_PIN[i] >= 0) 
      Orion.stopPulse(IN_SERVO_PIN[i]);
  }

}



//=======================================================================================================
// InitArduinoServos
//=======================================================================================================
void InitArduinoServos()
{

  for(int i=0;i<18;i++)
   {
   Orion.setSpeed(2000);
   Orion.setAngle(ALL_SERVO_PIN[i],Aoffsets[i]);
   Orion.execute();
   
   delay(1000); 
   }
  unsigned int key = (int)((EEPROM.read(510)<<8) | EEPROM.read(511));
  if(key==EEKEY){
 
     
    Serial.println("Arduino Servos Init");
  }
}



//=======================================================================================================
// QueryServoFBAngle
//=======================================================================================================
int QueryServoFBAngle(int iPin) {
  if (iPin >= 0)
    return Orion.queryFBAngle(iPin);

 
}    



//=======================================================================================================
// PrintServoInfo
//=======================================================================================================
/*void PrintServoInfo() {
  // Will print out information about each of the servos...
  int i;
  for (int j=0; j < 18; j++) {
    Serial.print(i = IN_SERVO_PIN[j], DEC);
    Serial.print("(");
    Serial.print(IKPinsNames[j]);
    Serial.print(")");
    Serial.print(":");
    Serial.print(QueryServoFBAngle(i), DEC);
    if ( i >= 0) {
      Serial.print(" ");
      Serial.print(Orion.queryForce(i), DEC);
      Serial.print(" : ");
      Serial.print(Orion.queryAOffset(i), DEC);
      Serial.print("(" );
      Serial.print(Orion.queryServoMin(i), DEC);
      Serial.print(",");
      Serial.print(Orion.queryServoMax(i), DEC);
      Serial.print(")");
    } 
    
    if ((j % 3) == 2)
      Serial.println("");
    else  
      Serial.print("\t");
  }
}
*/

