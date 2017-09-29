

//Data
#include <EasyTransfer.h>
#define HWSERIAL Serial1

//////MOTOR & Pot STUFF///////////////////////////////
#include <AccelStepper.h>



AccelStepper stepper(1, 3, 4); //Syntax: Shield, Pin 3, Pin4

int target = 0; //AnalogReadTarget
const int SLEEP = 14; //Pin DEF
//int pos = 3600; //stepper range

//0 - 1023 resolution measured angles
//const int potpos0 = 524; //horizontal grip, 
const int potpos0 = 104; //vertical
const int potpos1 = 220;
const int potpos2 = 194;
const int potpos3 = 168; //math 576 REMEASURE!!!!!
const int potpos4 = 142;
const int potpos33 = 167;
const int potpos22 = 201;

volatile int potpos = 0;
volatile int sensorValue = 0;
volatile int curpos = 0; //AnalogReagPos after positioning routine
volatile long timerT0 = 0;
volatile long timerT1 = 0;
volatile int  RealPos = 0;
volatile int  PosRef  = 0;
int PosMath = 0;


//Data
EasyTransfer Tranny;

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  long rotsend;
  int possend;
  //int tNUM;
  
};
SEND_DATA_STRUCTURE rpmcData;


///////////////Structuring/////////////////////////////////
volatile int mode = 0; // 0 is boot, 1 is read array and find position, 2 ist measurment mode Pot read on and touch read on, 3 = stop, 4 = report
int activetrial = 0;



//MISC
#define TOLERANCE 1

//LIGHT Control
//#define LIGHTON_PIN 15
#define LIGHTOFF_PIN 16 
//const int LIGHTON_PIN = 15;
//const int LIGHTOFF_PIN = 16;



void setup() {
  pinMode(A9, INPUT);
  
  //MOTOR AND POT///////////////////////////
  //Serial.begin(9600);
 pinMode(A9, INPUT);
 
 
 //pinMode(LIGHTON_PIN, OUTPUT);
 //digitalWrite(LIGHTON_PIN, HIGH);
 pinMode(LIGHTOFF_PIN, OUTPUT);
 digitalWrite(LIGHTOFF_PIN, HIGH);
  
  
  pinMode(SLEEP, OUTPUT);
  stepper.setMaxSpeed(8000);
  stepper.setAcceleration(8000);

  Tranny.begin(details(rpmcData), &Serial1);
  Serial1.begin(115200); //SERIAL to other Teensy max 115200
      
}


void loop() {
sensorValue = analogRead(A9);

/*
//sensorValue = map(analogRead(A9), 0, 1023, 0, 3599);
Serial.begin(9600);
Serial.print(sensorValue);
Serial.print("\t");
delay(500);
*/
///// uncomment above to calibrate pot 

if (mode == 0) {
    //digitalWrite(LIGHTON_PIN, HIGH);
    digitalWrite(LIGHTOFF_PIN, HIGH);
    digitalWrite(SLEEP, LOW); //SLEEP low enables resistancefree turning
    stepper.setCurrentPosition(0);   
  }
  
if (Serial1.available()){        //serial listen for 1-16 from stopwatch teensy and startShow (1-16)
   activetrial = Serial1.read(); 
    mode = 1;
    }

  ////FIND start position
  if (mode == 1) {
      //delay(11);
      //Serial1.clear();
      //rpmcData.rotsend = 0;
      //rpmcData.possend = 0;
      //rpmcData.tNUM = activetrial;
      //Tranny.sendData();
      //Serial1.flush();
         
    Positioning();   
}    

  //////////////POT TIMER
  if (mode == 2) {
    noInterrupts();
    Timer();
  
    }
   
  ///////// Haptic feedback "target stops" in measurement mode
  if (mode == 3) {
   noInterrupts();
 TimerStop();
  }



// Benchmark
/*int ENDE = ANFANG - micros();
Serial.begin(9600);
Serial.print(ENDE);
Serial.print("\t");
//delay(500);
*/

 
  
}


void Positioning() {
 if (activetrial == 1 || activetrial == 2 || activetrial == 3 || activetrial == 4 || activetrial == 9 || activetrial == 10 || activetrial == 11 || activetrial == 12) {
      stepper.stop();
      if (sensorValue <= potpos2) target = abs(sensorValue - potpos2) * 80; //positiv = counter clockwise
      if (sensorValue > potpos2) target = -abs(sensorValue - potpos2) * 80;
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.setAcceleration(8000);
      stepper.setSpeed(8000);
      stepper.runToNewPosition(target);
      delay(20);
      curpos = analogRead(A9);
      
    }


    else if (activetrial == 5 || activetrial == 6 || activetrial == 7 || activetrial == 8 || activetrial == 13 || activetrial == 14 || activetrial == 15 || activetrial == 16) {
      stepper.stop();
      if (sensorValue <= potpos3) target = abs(sensorValue - potpos3) * 80; //positiv = counter clockwise
      if (sensorValue > potpos3) target = -abs(sensorValue - potpos3) * 80;
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.setAcceleration(8000);
      stepper.setSpeed(8000);
      stepper.runToNewPosition(target);
      delay(20);
      curpos = analogRead(A9);
      
      
    }

     else if (activetrial == 17){
      stepper.stop();
      if (sensorValue <= potpos0) target = abs(sensorValue - potpos0) * 80; //positiv = counter clockwise
      if (sensorValue > potpos0) target = -abs(sensorValue - potpos0) * 80;
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.setAcceleration(8000);
      stepper.setSpeed(8000);
      stepper.runToNewPosition(target);
     
     }

    if (stepper.distanceToGo() == 0) {
      stepper.setCurrentPosition(0);
      delay(20);
      //digitalWrite(LIGHTON_PIN, LOW); //turns lights ON
      digitalWrite(SLEEP, LOW);
      mode = 2; //measurement mode
      
      /*
      Serial.begin(9600);
      Serial.println("start:");
      Serial.print(analogRead(A9));
      Serial.print("-");
      Serial.print(curpos);
      Serial.print("\t");
      */
  }
 
  } 
  

void Timer()  {

     if ((activetrial == 2 || activetrial == 3 || activetrial == 9 || activetrial == 12) && (analogRead(A9) > (curpos + TOLERANCE))) {
      timerT0 = micros();
      mode = 3;
    }
    
    else if ((activetrial == 5 || activetrial == 8 || activetrial == 14 || activetrial == 15) && (analogRead(A9) > (curpos + TOLERANCE))) {
      timerT0 = micros();
      mode = 3;
    }
    
    else if ((activetrial == 1 || activetrial == 4 || activetrial == 10 || activetrial == 11) && (analogRead(A9) < (curpos - TOLERANCE))) {
      timerT0 = micros();
      mode = 3;
    }
    
    else if ((activetrial == 6 || activetrial == 7 || activetrial == 13 || activetrial == 16) && (analogRead(A9) < (curpos - TOLERANCE))) {
      timerT0 = micros();
      mode = 3;
    }
    
    else if (activetrial == 17){
      digitalWrite(SLEEP, HIGH);
      stepper.stop();
    }
}

//////////////////////////////////
void TimerStop()  {
     if ((activetrial == 2 || activetrial == 3 || activetrial == 9 || activetrial == 12) && (analogRead(A9) >= potpos1)) { 
      timerT1 = (micros() - timerT0);
      PosRef =  potpos1;
      PosMath = 1;
      HapticStop();
    }
    
    else if ((activetrial == 5 || activetrial == 8 || activetrial == 14 || activetrial == 15) && (analogRead(A9) >= potpos2)) { //ex22
      timerT1 = (micros() - timerT0);
      PosRef = potpos2;
      PosMath = 1;
      HapticStop();
    }
    
    else if ((activetrial == 1 || activetrial == 4 || activetrial == 10 || activetrial == 11) && (analogRead(A9) <= potpos3)) { //ex33
      timerT1 = (micros() - timerT0);
      PosRef = potpos3;
      PosMath = -1;
      HapticStop();
    }
    
    else if ((activetrial == 6 || activetrial == 7 || activetrial == 13 || activetrial == 16) && (analogRead(A9) <= potpos4)) {
      timerT1 = (micros() - timerT0);
      PosRef = potpos4;
      PosMath = -1;
      HapticStop();
    }   
}
//////////////////////////////////  
void HapticStop() {
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.stop();
      digitalWrite(LIGHTOFF_PIN, LOW);
      interrupts();
      Report();
  }
////////////////////////////////////
void Report() {
    delay(50);
    curpos = analogRead(A9);
    RealPos = (curpos-PosRef)*PosMath;

  
  rpmcData.rotsend = timerT1;
  rpmcData.possend = RealPos;
  //rpmcData.tNUM = 0;
  Tranny.sendData();
  
  //digitalWrite(LIGHTOFF_PIN, HIGH);
  //digitalWrite(LIGHTON_PIN, HIGH);
  delay(800); //increase freeze time til sleep HIGH
  mode = 0;
     
}







