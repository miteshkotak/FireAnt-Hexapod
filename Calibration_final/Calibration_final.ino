// This is simple example how feedback servos works
// First servo is in calibration mode and second repeats the motions
#include <EEPROM.h>
#include <Orion.h>
#include <SPI.h>

#define DEGREE 176  //servo units/degree
#define EEKEY 0x1234  //eeprom key to determine if arduino eeprom has valid data

const unsigned int IKLengths[18] = {525,802,1283,
                                    525,802,1283,
                                    525,802,1283,
                                    525,802,1283,
                                    525,802,1283,
                                    525,802,1283};

const unsigned char reverse[18] = {0,1,0,
                                   0,1,0,
                                   0,1,0,
                                   1,0,1,
                                   1,0,1,
                                   1,0,1
                                   };


const int SERVO_MIN = -1000;
const int SERVO_MAX = 1000;

const int IN_SERVO_PIN[18] = {23,22,21,20,19,18,17,16,0,15,14,13,12,11,10,9,8,1};

const int BUTTON_PIN = BTNA;
bool calibrated;

int buttonState = 0;         // variable for reading the pushbutton status 


 
void setup()
{
  Serial.begin( 9600 );
  Serial.print( "Calibration Test ...\n" );
//asm volatile ("jmp 0x0000"); //reset
  Orion.begin(); 
  
  //Orion.resetRegs();
  
  Orion.tone(NOTE_C6,100); 
  
  
  //
for(int i=0;i<18;i++){
  Orion.stopPulse( IN_SERVO_PIN[i] ); // it is by default, but this way it is more clear
  //Orion.setPOffset( IN_SERVO_PIN[i], 0 ); // P offset
 // Orion.setAOffset( IN_SERVO_PIN[i], 0 );/// A offset
  
  Orion.setServoMin( IN_SERVO_PIN[i], SERVO_MIN );
  Orion.setServoMax( IN_SERVO_PIN[i], SERVO_MAX );
  //Serial.print(IN_SERVO_PIN[i]);
}
  pinMode( BUTTON_PIN, INPUT);   
  calibrated = true; // calibration disabled ... not working ... was false;*/

}

void loop()
{
   Orion.red( true ); // some indication
    if( BTNA )
    {
     
  for(int i=0;i<18;i++){
    Orion.setServoDegree(IN_SERVO_PIN[i],DEGREE);
    Orion.setServoDir(IN_SERVO_PIN[i],reverse[i]);
    Orion.setServoIKPin(i/4,i%4,IN_SERVO_PIN[i]);
    Orion.setServoIKLength(i/4,i%4,IKLengths[i]);
    }    
      
      Orion.green( true );
      for (int i=0;i<18;i++){
      Orion.setPOffset( IN_SERVO_PIN[i], Orion.queryFBAngle( IN_SERVO_PIN[i] ) );   //Set P offset
      Orion.setAOffset( IN_SERVO_PIN[i], Orion.queryFBAngle( IN_SERVO_PIN[i] ) );    //Set A offset
      int Servomax=  (Orion.queryFBAngle( IN_SERVO_PIN[i] ))+1000;
      int Servomin=  (Orion.queryFBAngle( IN_SERVO_PIN[i] ))-1000;
      
      Orion.setServoMin( IN_SERVO_PIN[i], Servomin );
      Orion.setServoMax( IN_SERVO_PIN[i],Servomax );
     
      }

        for(int i = 0;i<6;i++){
      Orion.setServoIKDir(i,0);
  }


      Orion.writeRegisters();
       
      EEPROM.write(510,EEKEY>>8);
      EEPROM.write(511,EEKEY);
      Orion.red( false );
 
for (int i=0;i<18;i++){
      Serial.print(Orion.queryPOffset(IN_SERVO_PIN[i]));
     Serial.print("\n"); 
      Serial.print(Orion.queryAOffset(IN_SERVO_PIN[i]));
      Serial.print(Orion.queryServoMin(IN_SERVO_PIN[i]));
      Serial.print(Orion.queryServoMax(IN_SERVO_PIN[i]));
}
      calibrated = true;
     
    }
  
}
