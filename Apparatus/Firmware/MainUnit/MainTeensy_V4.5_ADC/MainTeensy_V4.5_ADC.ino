


//TOdo!! ATTACH interrupts to ADC values (potpos) to improve timing accuracy

//ADC
//A10 is now A10!!!!! Light off Pin now PIN 15!!!!!!
// to use both ADCs move from A10 (Pin 23; ADC0) to A3 (Pin17; ADC0 and ADC1); A10 is now reference
#include <ADC.h>

ADC *adc = new ADC(); // adc object



//Data
#include <EasyTransfer.h>
#define HWSERIAL Serial1


//////MOTOR & Pot STUFF///////////////////////////////
#include <AccelStepper.h>
AccelStepper stepper(1, 3, 4); //Syntax: Shield, Pin 3, Pin4

int target = 0; //AnalogReadTarget
const int SLEEP = 14; //Pin DEF
//int pos = 3600; //stepper range
/*
//0 - 1023 resolution measured angles 10-bit
//const int potpos0 = 524; //horizontal grip, 
const int potpos0 = 104; //vertical
const int potpos1 = 220;
const int potpos2 = 194;
const int potpos3 = 168; //math 576 REMEASURE!!!!!
const int potpos4 = 142;

*/

//12-bit numbers 0 4095

const int potpos0 = 416; //vertical
const int potpos1 = 880;
const int potpos2 = 776;
const int potpos3 = 672; //math 576 REMEASURE!!!!!
const int potpos4 = 568;


volatile int potpos = 0;
volatile int sensorValue = 0;
volatile int curpos = 0; //AnalogReagPos after positioning routine
volatile long timerT0 = 0;
volatile long timerT1 = 0;
volatile int  RealPos = 0;
volatile int  PosRef  = 0;
volatile int PosMath = 0;


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
volatile int activetrial = 0;



//MISC
#define TOLERANCE 1

//LIGHT Control
//#define LIGHTON_PIN 15
#define LIGHTOFF_PIN 15 //NEW!
//const int LIGHTON_PIN = 15;
//const int LIGHTOFF_PIN = 16;




void setup() {
  //analogReadResolution(12);
  ///////////////////ADC parameters////////////////////
  pinMode(A11, INPUT); //from pot NEGATIVE
  pinMode(A10, INPUT); //from ground POSITIVE
  

  //ADC_0
  adc->setAveraging(32, ADC_0); // set number of averages
  adc->setResolution(12, ADC_0); // set bits of resolution
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    adc->setConversionSpeed(ADC_MED_SPEED, ADC_0); // change the conversion speed
    adc->setSamplingSpeed(ADC_HIGH_SPEED, ADC_0); // change the sampling speed
    //adc->enableCompare(-1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < -1.0V

  //ADC_1
  adc->setAveraging(32, ADC_1); // set number of averages
  adc->setResolution(12, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_HIGH_SPEED, ADC_1); //change the conversion speed
  adc->setSamplingSpeed(ADC_HIGH_SPEED, ADC_1); // change the sampling speed
  adc->enableCompare(-1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < -1.0V

  //MOTOR AND POT///////////////////////////
  //Serial.begin(9600);

 
 
 //pinMode(LIGHTON_PIN, OUTPUT);
 //digitalWrite(LIGHTON_PIN, HIGH);
 pinMode(LIGHTOFF_PIN, OUTPUT);
 digitalWrite(LIGHTOFF_PIN, HIGH);
  
  
  pinMode(SLEEP, OUTPUT);
  stepper.setMaxSpeed(8000);
  stepper.setAcceleration(8000);

  Tranny.begin(details(rpmcData), &Serial1);
  Serial1.begin(115200); //SERIAL to other Teensy max 115200

  delay(10);

  digitalWrite(SLEEP, LOW); //turn off engine (no humm)
   digitalWrite(LIGHTOFF_PIN, HIGH);   
}


void loop() {
//Loop BENCHMARK BEGIN
//int ANFANG = micros();

  
sensorValue = adc->analogReadDifferential(A10, A11, ADC_0);




/*
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

/*
int ENDE = micros() - ANFANG;
Serial.begin(9600);
Serial.print("sensorValue:");
Serial.print("\t");
Serial.print(sensorValue);
Serial.print("\t");
Serial.print("LOOP-Time:");
Serial.print(ENDE);
Serial.print("\t");
Serial.print("curpos:");
Serial.print(curpos);
Serial.print("\t");

Serial.print("FANCY_analogRead:");
//Serial.print(adc->analogSyncReadDifferential(A11, A10, ADC_0, ADC_1));
//Serial.print(adc->analogRead(A11));
//Serial.print("\t");
//Serial.print(adc->startContinuousDifferential(A11, A10, ADC_0));
//adc->startContinuousDifferential(A10, A11, ADC_0);
//Serial.print(adc->analogRead(A10));
//Serial.print("\t");
Serial.print(adc->analogRead(A10));
Serial.print("\t");

Serial.print("simple_analogRead:");
Serial.print(analogRead(A10));
Serial.print("\t");

Serial.print("RANGE:");
Serial.print(adc->getMaxValue(ADC_0));
Serial.print("\t");

//Serial.print("alive");
delay(1000);
*/
  
}


void Positioning() {
 if (activetrial == 1 || activetrial == 2 || activetrial == 3 || activetrial == 4 || activetrial == 9 || activetrial == 10 || activetrial == 11 || activetrial == 12) {
      stepper.stop();
      if (sensorValue <= potpos2) target = abs(sensorValue - potpos2) * 20; //positiv = counter clockwise
      if (sensorValue > potpos2) target = -abs(sensorValue - potpos2) * 20;
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.setAcceleration(8000);
      stepper.setSpeed(8000);
      stepper.runToNewPosition(target);
      delay(20);
      curpos = adc->analogReadDifferential(A10, A11, ADC_0);
      
    }


    else if (activetrial == 5 || activetrial == 6 || activetrial == 7 || activetrial == 8 || activetrial == 13 || activetrial == 14 || activetrial == 15 || activetrial == 16) {
      stepper.stop();
      if (sensorValue <= potpos3) target = abs(sensorValue - potpos3) * 20; //positiv = counter clockwise
      if (sensorValue > potpos3) target = -abs(sensorValue - potpos3) * 20;
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.setAcceleration(8000);
      stepper.setSpeed(8000);
      stepper.runToNewPosition(target);
      delay(20);
      curpos = adc->analogReadDifferential(A10, A11, ADC_0);
      
      
    }

     else if (activetrial == 17){
      stepper.stop();
      if (sensorValue <= potpos0) target = abs(sensorValue - potpos0) * 20; //positiv = counter clockwise
      if (sensorValue > potpos0) target = -abs(sensorValue - potpos0) * 20;
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.setAcceleration(8000);
      stepper.setSpeed(8000);
      stepper.runToNewPosition(target);
     
     }


      else if (activetrial == 30){
      stepper.stop();
      if (sensorValue <= potpos2) target = abs(sensorValue - potpos2) * 20; //positiv = counter clockwise
      if (sensorValue > potpos2) target = -abs(sensorValue - potpos2) * 20;
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.setAcceleration(8000);
      stepper.setSpeed(8000);
      stepper.runToNewPosition(target);
     
     }

      else if (activetrial == 31){
      stepper.stop();
      if (sensorValue <= potpos3) target = abs(sensorValue - potpos3) * 20; //positiv = counter clockwise
      if (sensorValue > potpos3) target = -abs(sensorValue - potpos3) * 20;
      digitalWrite(SLEEP, HIGH); //WAKE UP
      stepper.setAcceleration(8000);
      stepper.setSpeed(8000);
      stepper.runToNewPosition(target);
     
     }

      else if (activetrial == 27){
      digitalWrite(SLEEP, LOW);
      delay(1000); 
      mode = 0;
      
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
      Serial.print(analogRead(A10));
      Serial.print("-");
      Serial.print(curpos);
      Serial.print("\t");
      */
  }
 
  } 
  
//replace mode with interrupt setting => rising or falling
void Timer()  {
  
    adc->startContinuousDifferential(A11, A10, ADC_1);

     if ((activetrial == 2 || activetrial == 3 || activetrial == 9 || activetrial == 12) && (adc->analogReadContinuous(A10) > (curpos + TOLERANCE))) {
      timerT0 = micros();
      mode = 3;
    }
    
    else if ((activetrial == 5 || activetrial == 8 || activetrial == 14 || activetrial == 15) && (adc->analogReadContinuous(A10) > (curpos + TOLERANCE))) {
      timerT0 = micros();
      mode = 3;
    }
    
    else if ((activetrial == 1 || activetrial == 4 || activetrial == 10 || activetrial == 11) && (adc->analogReadContinuous(A10) < (curpos - TOLERANCE))) {
      timerT0 = micros();
      mode = 3;
    }
    
    else if ((activetrial == 6 || activetrial == 7 || activetrial == 13 || activetrial == 16) && (adc->analogReadContinuous(A10) < (curpos - TOLERANCE))) {
      timerT0 = micros();
      mode = 3;
    }
    
    else if (activetrial == 17 || activetrial == 30 || activetrial == 31){
      digitalWrite(SLEEP, HIGH);
      stepper.stop();
    }

    adc->stopContinuous();
}

//////////////////////////////////
void TimerStop()  {
    adc->startContinuousDifferential(A10, A11, ADC_0);
  
     if ((activetrial == 2 || activetrial == 3 || activetrial == 9 || activetrial == 12) && (adc->analogReadContinuous(A10) >= potpos1)) {
      timerT1 = (micros() - timerT0);
      adc->stopContinuous();
      PosRef =  potpos1;
      PosMath = 1;
      HapticStop();
    }
    
    else if ((activetrial == 5 || activetrial == 8 || activetrial == 14 || activetrial == 15) && (adc->analogReadContinuous(A10)>= potpos2)) {
      timerT1 = (micros() - timerT0);
      adc->stopContinuous(); 
      PosRef = potpos2;
      PosMath = 1;
      HapticStop();
    }
    
    else if ((activetrial == 1 || activetrial == 4 || activetrial == 10 || activetrial == 11) && (adc->analogReadContinuous(A10) <= potpos3)) {
      timerT1 = (micros() - timerT0);
      adc->stopContinuous(); 
      PosRef = potpos3;
      PosMath = -1;
      HapticStop();
    }
    
    else if ((activetrial == 6 || activetrial == 7 || activetrial == 13 || activetrial == 16) && (adc->analogReadContinuous(A10) <= potpos4)) {
      timerT1 = (micros() - timerT0);
      adc->stopContinuous();
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
    curpos = adc->analogReadDifferential(A10, A11, ADC_0);
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







