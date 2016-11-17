#define FILTER
#define DEGREE 176  //servo units/degree
#define EEKEY 0x1234  //eeprom key to determine if arduino eeprom has valid data

//leg servo pin assignments
const int IKPins[29] = {LFC,LFF,LFT,-1,
                        LMC,LMF,LMT,-1,
                        LRC,LRF,LRT,-1,
                        RFC,RFF,RFT,-1,
                        RMC,RMF,RMT,-1,
                        RRC,RRF,RRT,-1,
                        HeadPitchPin,HeadRollPin,HeadYawPin,PincerLPin,PincerRPin};

unsigned char Gait;
unsigned char RotateMode;
unsigned char RightJoystickMode;
unsigned char ReturnToCenter;
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
int PincerL;
int PincerR;
int HeadPitch;
int HeadRoll;
int HeadYaw;
int TailPitch;
int TailYaw;

int HeadPitchOffset;
int HeadRollOffset;
int HeadYawOffset;

boolean onoff;
boolean heartbeat;
long lastbeat; 
boolean servoatlimit;
boolean gripped;

#ifdef FILTER
  //analog ps2 control filter variables
  long rxfilter;
  long ryfilter;
  long lxfilter;
  long lyfilter;
  long dxfilter;
  long dyfilter;
#endif

//simple signal filter macro
#define filter_update(filter,input,rate) (filter = filter - (filter >> rate) + input) >> rate;

BMPS2 ps2x(6,100);
BMServo TailYawServo(9);
BMServo TailPitchServo(10);

#ifdef DEMO
  boolean demomode;
  long walktotal;
#endif
