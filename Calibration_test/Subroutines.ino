void Initpostion()
{
  
  for(int i=3;i<4;i++)
   {
   Orion.setSpeed(2000);
   Orion.setAngle(IN_SERVO_PIN[i],Aoffsets[i]);
   Orion.execute();
   /*Serial.print(Orion.queryFBAngle(IN_SERVO_PIN[i]));
   Serial.print("\n");
   Serial.print(Orion.queryPOffset(IN_SERVO_PIN[i]));
   Serial.print("\n");
   Serial.print(Orion.queryAOffset(IN_SERVO_PIN[i]));
   Serial.print("\n");
   /*Serial.print( Orion.queryServoMin(IN_SERVO_PIN[i]));
   Serial.print("\n");
   Serial.print( Orion.queryServoMax(IN_SERVO_PIN[i]));
   Serial.print("\n");*/
   delay(500);
   }
for(int i=3;i<4;i++)
   {
   Serial.print(Orion.queryFBAngle(IN_SERVO_PIN[i]));
   Serial.print("\n"); 
   }

}

