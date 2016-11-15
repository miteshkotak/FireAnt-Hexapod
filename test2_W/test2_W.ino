//=======================================================================================================
// Mitesh's Ant test program. 
//=======================================================================================================
//#define CUSTOMPINS

//#define DEBUG      //include debug serial outputs

#include <EEPROM.h>
#include <BMServo.h>
#include <BMSerial.h>
#include <BMPS2.h>
#include <SPI.h>
#include <Orion.h>

#define DEBUG
#define CALIBRATE
#define NORMAL

#define DEGREE 178//134
#define EEKEY 0x1234  //eeprom key to determine if arduino eeprom has valid data

//Servo pin numbers
 

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


#define DEADZONE 10

#define GREENLED A1
#define REDLED A0

//Maximum body movement offsets
#define MAXX 625
#define MAXY 2000
#define MAXZ 550
#define MAXR 175
#define MAXBODYYAW 300
#define MAXBODYROLL 300
#define MAXBODYPITCH 300

const unsigned char servoPins[] = { LFC, LFF, LFT,   LMC, LMF, LMT,   LRC, LRF, LRT, 
                                    RFC, RFF, RFT,   RMC, RMF, RMT,   RRC, RRF, RRT,
                                    HeadRollPin, HeadYawPin, HeadPitchPin, PincerLPin, PincerRPin }; 
const unsigned int NUM_SERVOS = sizeof(servoPins)/sizeof(unsigned char); 


//Robot Initial Positions(1/10 mm units)
const int DefBodyPosX[6] = {706,706,706,-706,-706,-706};
const int DefBodyPosY[6] = {-370,-370,-370,-370,-370,-370};
const int DefBodyPosZ[6] = {841,0,-841,841,0,-841};
const int DefLegPosX[6] = {1083,1250,1083,-1083,-1250,-1083};
const int DefLegPosY[6] = {0,0,0,0,0,0};
const int DefLegPosZ[6] = {625,0,-625,625,0,-625};


// blinking green LED
bool walking;
boolean heartbeat; 
long lastbeat;
boolean gripped;
// FireAnt body
unsigned char Gait;
int Rate;

int Xdist;
int Ydist;
int Zdist;
int Rdist;
int BodyOffsetX;
int BodyOffsetY;
int BodyOffsetZ;
int BodyRotOffsetZ;
int BodyYaw;
int BodyPitch;
int BodyRoll;
int XLegAdj;
int ZLegAdj; 


#define DEADZONE 10

#define GREENLED A1
#define REDLED A0
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

unsigned char reverse[] = {
  0,1,0,
  0,1,0,
  0,1,0,
  1,0,1,
  1,0,1,
  1,0,1,
  0,0,0,0,1,
  0, 0};

const char* IKPinsNames[] = {
  "LFC","LFF","LFT",
  "LMC","LMF","LMT",
  "LRC","LRF","LRT",
  "RFC","RFF","RFT",
  "RMC","RMF","RMT",
  "RRC","RRF","RRT",
  "Head Pitch(UD)","Head Roll (ROT)","Head Yaw (LR)","L Pincer", "R Pincer", 
  "Tail Yaw(LR)", "Tail Pitch(UD"};

boolean fUseServoOffsets;
const int ServoOffsets[] = {
  0, 900,0,
  0, 900,0,
  0, 900,0,
  0, 900,0,
  0, 900,0,
  0, 900,0,
  0,0,0,0,0,
  0, 0};

int aiPositions[CNTPINS];
boolean onoff = false;
byte iNextPin = 0xff;
int PinVal = 0;
int PincerPos = 0;

//BMPS2 ps2x(6,100);

// Define the Tail servos which are run by the Arduino
BMServo TailYawServo(9);
BMServo TailPitchServo(10);


// foreword references
extern boolean DebugMonitor();
boolean g_fDebugOutput = false;

void StopServos()
{
  int i;
  
  for( i = 0; i < NUM_SERVOS; i++ )
    Orion.stopPulse( servoPins[i] ); // it is by default, but this way it is more clear 
    Serial.println("  STOP ORION  ");
}

void InitState()
{
//  RightJoystickMode=0;
//  ReturnToCenter=0;
//  RotateMode=0;
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

 for( int i=0; i<6; i++ )
  {
    Orion.setBodyPos(i,DefBodyPosX[i],DefBodyPosY[i],DefBodyPosZ[i]);
    Orion.setLegPos(i,DefLegPosX[i],DefLegPosY[i],DefLegPosZ[i]);
  }

   Gait = OrionClass::TRI4;
  Orion.gaitSelect((OrionClass::GAITTYPE)Gait); 
}
 
//#ifdef DEMO
boolean Walk(int xdist,int zdist,int rdist,int ydist,int dist)
{
  Xdist = xdist;
  Zdist = zdist;
  Rdist = rdist;
  Ydist = ydist;
  walktotal+=abs(xdist)+abs(zdist)+abs(rdist);
  if(walktotal>=dist){
     walktotal-=dist;
     return true;
  }
  return false;
}

#define CodeStep(num,code) case num: code; state=num+1; break;
#define Step(num,move) case num: if(move) state=num+1; break;


void Demo_State()
{
  int walkcnt=0;
  
  delay(2000);
  
  int state=0;
  while(1){
    TailPitchServo.update();
    TailYawServo.update();
   // if(ps2x.isUpdated()){
     /* if(ps2x.buttonPressed(PSB_START)){
        StopServos();
        delay(100); //give servos time to stop
        asm volatile ("jmp 0x0000");  //reset
      }*/

      switch(state){
      Step(0,CalcBodyOffsetY(80,800))
      Step(1,CalcBodyRoll(20,-100) & CalcBodyPitch(20,-100))
      Step(2,CalcBodyPitch(20,100))
      Step(3,CalcBodyRoll(20,100))
      Step(4,CalcBodyPitch(20,-100))
      Step(5,CalcBodyRoll(20,-100))
      Step(6,CalcBodyRoll(20,0) & CalcBodyPitch(20,0))
      Step(7,CalcBodyPitch(20,100) & CalcBodyOffsetZ(100,-500) & CalcBodyYaw(20,100) & OpenPincers(100,500))
      Step(8,CalcBodyPitch(20,-100) & CalcBodyOffsetZ(100,500) & CalcBodyYaw(20,0)  & ClosePincers(100,0))
      Step(9,CalcBodyPitch(20,100) & CalcBodyOffsetZ(100,-500) & CalcBodyYaw(20,-100)  & OpenPincers(100,500))
      Step(10,CalcBodyPitch(20,-100) & CalcBodyOffsetZ(100,500) & CalcBodyYaw(20,0)  & ClosePincers(100,0))
      Step(11,CalcBodyPitch(20,100) & CalcBodyOffsetZ(100,-500) & OpenPincers(100,500))      
      Step(12,CalcBodyPitch(20,0) & CalcBodyOffsetZ(100,0) & OpenPincers(100,0))

      Step(13,Walk(100,0,0,MAXY/8,400));
      Step(14,Walk(200,0,0,MAXY/6,400) & OpenPincers(100,500));
      Step(15,Walk(400,0,0,MAXY/4,400));
      Step(16,Walk(MAXX,0,0,MAXY/4,10000));
      Step(17,Walk(400,0,0,MAXY/4,400));
      Step(18,Walk(200,0,0,MAXY/8,400) & ClosePincers(100,0));
      Step(19,Walk(100,0,0,MAXY/6,400));

      Step(20,Walk(-100,0,0,MAXY/8,400));
      Step(21,Walk(-200,0,0,MAXY/6,400) & OpenPincers(100,500));
      Step(22,Walk(-400,0,0,MAXY/4,400));
      Step(23,Walk(-MAXX,0,0,MAXY/4,10000));
      Step(24,Walk(-400,0,0,MAXY/4,400));
      Step(25,Walk(-200,0,0,MAXY/8,400) & ClosePincers(100,0));
      Step(26,Walk(-100,0,0,MAXY/6,400));
      
      Step(27,Walk(0,0,20,MAXY/8,100));
      Step(28,Walk(0,0,40,MAXY/6,100) & OpenPincers(100,500));
      Step(29,Walk(0,0,80,MAXY/4,100));
      Step(30,Walk(0,0,MAXR,MAXY/4,2500));
      Step(31,Walk(0,0,80,MAXY/4,100));
      Step(32,Walk(0,0,40,MAXY/8,100) & ClosePincers(100,0));
      Step(33,Walk(0,0,20,MAXY/6,100));

      Step(34,Walk(0,0,-20,MAXY/8,100));
      Step(35,Walk(0,0,-40,MAXY/6,100) & OpenPincers(100,500));
      Step(36,Walk(0,0,-80,MAXY/4,100));
      Step(37,Walk(0,0,-MAXR,MAXY/4,2500));
      Step(38,Walk(0,0,-80,MAXY/4,100));
      Step(39,Walk(0,0,-40,MAXY/8,100) & ClosePincers(100,0));
      Step(40,Walk(0,0,-20,MAXY/6,100));

      Step(41,Walk(0,0,0,MAXY/6,0))
      CodeStep(42,RotateMode=2);
      Step(43,CalcBodyPitch(20,100) & OpenPincers(100,500));
      Step(44,CalcBodyYaw(20,100));
      Step(45,CalcBodyYaw(20,-100));
      Step(46,CalcBodyYaw(20,100));
      Step(47,CalcBodyYaw(20,-100));
      Step(48,CalcBodyPitch(20,0)&CalcBodyYaw(20,0) & ClosePincers(100,0));

      CodeStep(49,RotateMode=1);
      Step(50,CalcBodyPitch(20,-100));
      Step(51,CalcBodyYaw(20,100));
      Step(52,CalcBodyYaw(20,-100));
      Step(53,CalcBodyYaw(20,100));
      Step(54,CalcBodyYaw(20,-100));
      Step(55,CalcBodyPitch(20,0)&CalcBodyYaw(20,0));
      
      CodeStep(56,RotateMode=2);
      Step(57,CalcBodyPitch(100,100) & OpenPincers(500,500));
      Step(58,CalcBodyYaw(100,100));
      Step(59,CalcBodyYaw(100,-100));
      Step(60,CalcBodyYaw(100,100));
      Step(61,CalcBodyYaw(100,-100));
      Step(62,CalcBodyPitch(100,0)&CalcBodyYaw(100,0) & ClosePincers(500,0));

      CodeStep(63,RotateMode=1);
      Step(64,CalcBodyPitch(100,-100));
      Step(65,CalcBodyYaw(100,100));
      Step(66,CalcBodyYaw(100,-100));
      Step(67,CalcBodyYaw(100,100));
      Step(68,CalcBodyYaw(100,-100));
      Step(69,CalcBodyPitch(100,0)&CalcBodyYaw(100,0));

      case 70:
        wait = 50;
        state=71;
        break;
      default:
        if(!wait--)
          state=0;
        break;
      };
      CalcAntAuto();  
      ProcessAnt();
      ProcessLegs();
    //}
  }
  
  delay(2000);
  int reversecnt=0;
  CalcBodyOffsetY(800,800);
  while(1){
    TailPitchServo.update();
    TailYawServo.update();
   // if(ps2x.isUpdated()){
     /* if(ps2x.buttonPressed(PSB_START)){
        StopServos();
        delay(100); //give servos time to stop
        asm volatile ("jmp 0x0000");  //reset
      }*/
  
      if(reversecnt==0){
        Xdist = MAXX;
        Zdist = 0;
        Rdist = 0;
        Ydist = MAXY/3;
        if(Orion.queryForce(LFC)<-1100){
          reversecnt=80;
        }
        if(Orion.queryForce(RFC)<-1100){
          reversecnt=-80;
        }
      }
      else{
        if(reversecnt>0){
          Xdist = -MAXX/2;
          Zdist = 0;
          Rdist = MAXR/4;
          Ydist = MAXY/3;
          reversecnt--;
        }
        else{
          Xdist = -MAXX/2;
          Zdist = 0;
          Rdist = -MAXR/4;
          Ydist = MAXY/3;
          reversecnt++;
        }
      }    
      CalcAntAuto();  
      ProcessAnt();
      ProcessLegs();
    }
  }
}
//#endif

 

void setup() {
  // put your setup code here, to run once:
 int servoPin;
  pinMode( servoPin, OUTPUT);  // Set servo pin as an output pin
  walking = false;
  heartbeat = true;
  lastbeat = millis();
  Serial.begin(9600);  
  Serial.println(" ORION BEGIN ");  

  
  Orion.begin();
  Orion.green( heartbeat );
  Orion.red(false); 
  Orion.tone(NOTE_C4,100);  
  Orion.tone(NOTE_D4,100);  
  Orion.tone(NOTE_E4,100);  
  Orion.tone(NOTE_F4,100);  
  
  InitState();
    
  StopServos();

 Demo_State(); 
  Orion.setBalanceMode(0x2);
  Orion.setBalanceTransRate(50);
  Orion.setBalanceAngleRate(200); 

  
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
  Orion.tone(NOTE_G4,200); 
  delay(500);
  Orion.tone(NOTE_G4,200); 




}

void loop() {
  // put your main code here, to run repeatedly:
if( !walking )
  {
   
      walking = true;
      InitState();
      
    }
    
    return;

 // move forward at given hegiht
  Xdist = MAXX/2;
  if( BodyOffsetY < MAXY/2 )
  {
    BodyOffsetY += MAXY/100;
    Xdist = 0; // do not walk, if you are not standing yet
 
  }
 
  
  //Heart beat LED
  long time = millis() - lastbeat;
  if( time > 200 )
  {
    lastbeat = millis();
    heartbeat ^= heartbeat;
    Orion.green( heartbeat );
  } 

  //Sends new commands to Orion if there are any changes
  Orion.execGait(BodyYaw,
                 BodyRoll,
                 BodyPitch,
                 0, //BodyOffsetX,
                 0, //BodyOffsetY,
                 0, //DefBodyRotOffset[RotateMode],
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

