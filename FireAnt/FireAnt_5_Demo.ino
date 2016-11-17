
#include <EEPROM.h>

#include <BMSerial.h>
#include <BMPS2.h>
#include <SPI.h>
#include <Orion.h>

#ifdef DEMO
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
void Demo()
{
/*  int walkcnt=0;
  
  delay(2000);
  
  int state=0;
  while(1){
    TailPitchServo.update();
    TailYawServo.update();
    if(ps2x.isUpdated()){
      if(ps2x.buttonPressed(PSB_START)){
        StopServos();
        delay(100); //give servos time to stop
        asm volatile ("jmp 0x0000");  //reset
      }

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
    }
  }*/
  
  delay(2000);
  int reversecnt=0;
  CalcBodyOffsetY(800,800);
  while(1){
    TailPitchServo.update();
    TailYawServo.update();
    if(ps2x.isUpdated()){
      if(ps2x.buttonPressed(PSB_START)){
        StopServos();
        delay(100); //give servos time to stop
        asm volatile ("jmp 0x0000");  //reset
      }
  
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
#endif

