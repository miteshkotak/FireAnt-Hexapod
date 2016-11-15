


// This is simple example how feedback servos works
// First servo is in calibration mode and second repeats the motions
#include<EEPROM.h>
#include <Orion.h>
#include <SPI.h>
#include<mathFrame.h>


#define LFC 23
#define LFF 22 
#define LFT 21
#define LMC 20
#define LMF 19
#define LMT 18
#define LRC 17
#define LRF 16
#define LRT 0
#define RFC 15
#define RFF 14
#define RFT 13
#define RMC 12
#define RMF 11
#define RMT 10
#define RRC 9
#define RRF 8
#define RRT 1
#define HeadRollPin 7
#define HeadYawPin 6
#define HeadPitchPin 5
#define PincerLPin 4
#define PincerRPin 3


#define TailYawPin  -9
#define TailPitchPin -10



#define EEKEY 0x1234

boolean fUseServoOffsets;
const int IN_SERVO_PIN[3] = {23,22,21};
const int OUT_SERVO_PIN[15] = {20,19,18,17,16,0,15,14,13,12,11,10,9,8,1};

const int BUTTON_PIN = BTNA;

const int ALL_SERVO_PIN[18] = {23,22,21,20,19,18,17,16,0,15,14,13,12,11,10,9,8,1};
const int Aoffsets[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


 

const char* IKPinsNames[] = {
  "LFC","LFF","LFT",
  "LMC","LMF","LMT",
  "LRC","LRF","LRT",
  "RFC","RFF","RFT",
  "RMC","RMF","RMT",
  "RRC","RRF","RRT",
  "Head Pitch(UD)","Head Roll (ROT)","Head Yaw (LR)","L Pincer", "R Pincer", 
  "Tail Yaw(LR)", "Tail Pitch(UD"};

//leg servo pin assignments
const int IKPins[] = {
  LFC,LFF,LFT,
  LMC,LMF,LMT,
  LRC,LRF,LRT,
  RFC,RFF,RFT,
  RMC,RMF,RMT,
  RRC,RRF,RRT,
  HeadPitchPin,HeadRollPin,HeadYawPin,PincerLPin,PincerRPin,
  TailYawPin, TailPitchPin};
#define CNTPINS (sizeof(IKPins)/sizeof(IKPins[0]))                   

 


 
void setup()
{
  Serial.begin( 9600 );
  Serial.print( "Calibration Test ...\n" );
Serial.println("***Ant Test Program***");   
  Orion.begin(); 
  Orion.tone(NOTE_C6,100);

  InitArduinoServos();
  delay(5000); 
      Serial.print(Orion.getZPos(IN_SERVO_PIN[0]));



  StopServos();
  Orion.tone(NOTE_C6,100);
  delay(1000);  // give some times for the serial port to init...
  // Check to see if it looks like the servo offsets are set or not...
  int sOffset;
  fUseServoOffsets = false;
  for(int i = 0;i<CNTPINS;i++) {
    if (IKPins[i] >= 0) {
      sOffset = Orion.queryAOffset(IKPins[i]);
      if (abs(sOffset) > 10) {
        fUseServoOffsets = true;
        break;
      }
    }
  }
  delay(500);
  Serial.println("***Ant Test Program***");
  if (fUseServoOffsets)
    {Serial.println("Servo Offsets Set");
    
    Serial.println(Orion.queryAOffset( IN_SERVO_PIN[0]));
    Serial.println(Orion.queryPOffset( IN_SERVO_PIN[0]));
    
    }
  else
    Serial.println("Servo Offsets are not set");
 
 

}

void loop()
{
  
  int angle_C,angle_F,angle_T;
  
  if( !BTNA )
  {
    
        
   delay(2000);
    angle_C = Orion.queryFBAngle( IN_SERVO_PIN[0] );
   
    //delay(2000); 
    // Serial.println (angle_C);
     //Serial.println ("\n");
    angle_F = Orion.queryFBAngle( IN_SERVO_PIN[1] );
    angle_T = Orion.queryFBAngle( IN_SERVO_PIN[2] );
    
     
      Orion.setTime( 2000 );

     /*
      Orion.setAngle( OUT_SERVO_PIN[0],10 );
     //Serial.println( Orion.queryFBAngle( OUT_SERVO_PIN[0] ));
     
      Orion.setAngle( OUT_SERVO_PIN[1], 10 );
      Orion.setAngle( OUT_SERVO_PIN[2], 10 );
    */
     
      Orion.setAngle( OUT_SERVO_PIN[3],angle_C );
      //Orion.setAngle( OUT_SERVO_PIN[4], 10 );
      Orion.setAngle( OUT_SERVO_PIN[5], angle_T );
      
      Orion.setAngle( OUT_SERVO_PIN[9],angle_C );
      Orion.setAngle( OUT_SERVO_PIN[10], angle_F );
      Orion.setAngle( OUT_SERVO_PIN[11], angle_T );

     
     
     Orion.execute(); 
      //Orion.setAngle( OUT_SERVO_PIN[9], angle[0] );
     // Orion.setAngle( OUT_SERVO_PIN[12], angle[0] );
      
    //Serial.print(Orion.getXPos(IN_SERVO_PIN[0]));
    //Serial.print(Orion.getYPos(IN_SERVO_PIN[0]));
    Serial.print(Orion.getZPos(IN_SERVO_PIN[0]));
    
    }
      
  
   
  //}

  
  /*else
  {
    Orion.red( true ); // some indication
    Balancemode(); 
  }*/
}

