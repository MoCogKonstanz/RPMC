 
#include <Adafruit_NeoPixel.h>

//Data
#include <EasyTransfer.h>
#define HWSERIAL Serial2

//comms
#include <Bounce.h>


//define 5 pins to mirror SuperLab/NI-DAQ 6501 digital port0 lines
#define digi1_PIN 1 // green
#define digi2_PIN 2 // yellow
#define digi3_PIN 3 // purple
#define digi4_PIN 4 // red
#define digi5_PIN 5 // blue
//black is GND

//NeoPixel
#define PIXEL_PIN 14    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_RGB + NEO_KHZ800);
volatile int showType = 0;


//LIGHTS
//#define LIGHTON_PIN 15 //interrupt attached to this pin in setup
#define LIGHTOFF_PIN 16 //interrupt attached to this pin in setup
//const int LIGHTON_PIN = 15;
//const int LIGHTOFF_PIN = 16;



//report + float math
volatile long ROTtime;
int fractional = 0;
int RealPos = 0;

//DAQ-comms
Bounce P0 = Bounce(digi1_PIN, 0);
Bounce P1 = Bounce(digi2_PIN, 0);
Bounce P2 = Bounce(digi3_PIN, 0);
Bounce P3 = Bounce(digi4_PIN, 0);
Bounce P4 = Bounce(digi5_PIN, 0);

//Data
EasyTransfer Tranny; 
struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  long rotsend;
  int possend;
  //int tNUM;  
};
RECEIVE_DATA_STRUCTURE rpmcData;


volatile int TRIAL = 0;

void setup() {


pinMode(digi1_PIN, INPUT_PULLUP);
pinMode(digi2_PIN, INPUT_PULLUP);
pinMode(digi3_PIN, INPUT_PULLUP);
pinMode(digi4_PIN, INPUT_PULLUP);
pinMode(digi5_PIN, INPUT_PULLUP);

//Data
Serial2.begin(115200); //max 115200
Tranny.begin(details(rpmcData), &Serial2);

//NeoPixel
//pinMode(LIGHTON_PIN, INPUT_PULLUP);
//attachInterrupt(digitalPinToInterrupt (LIGHTON_PIN), LightsON, FALLING); //when it goes low call lightsON funtion
pinMode(LIGHTOFF_PIN, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt (LIGHTOFF_PIN), LightsOFF, FALLING); //when it goes low call lightsOFF funtion
strip.setBrightness(128); 

strip.begin();
delay(20);
strip.show(); // Initialize all pixels to 'off'
}

void loop() {

//if { LINE combos 1-16 serial2 write 1-16 to other teensy}




P0.update();
P1.update();
P2.update();
P3.update();
P4.update();

if (P0.fallingEdge() && P1.fallingEdge() == false && P2.fallingEdge() == false && P3.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 1;
  trialcontrol();
  }
else if (P1.fallingEdge() && P0.fallingEdge() == false && P2.fallingEdge() == false && P3.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 2;
  trialcontrol();
  }
else if (P0.fallingEdge() && P1.fallingEdge() && P2.fallingEdge() == false && P3.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 3;
  trialcontrol();
  }
else if (P2.fallingEdge() && P0.fallingEdge() == false && P1.fallingEdge() == false && P3.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 4;
  trialcontrol();
  }
else if (P0.fallingEdge() && P2.fallingEdge() && P1.fallingEdge() == false && P3.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 5;
  trialcontrol();
  }
else if (P1.fallingEdge() && P2.fallingEdge() && P0.fallingEdge() == false && P3.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 6;
  trialcontrol();
  }
else if (P0.fallingEdge() && P1.fallingEdge() && P2.fallingEdge() && P3.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 7;
  trialcontrol();
  }
else if (P3.fallingEdge() && P0.fallingEdge() == false && P1.fallingEdge() == false && P2.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 8;
  trialcontrol();
  }
else if (P0.fallingEdge() && P3.fallingEdge() && P1.fallingEdge() == false && P2.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 9;
  trialcontrol();
  }
else if (P1.fallingEdge() && P3.fallingEdge() && P0.fallingEdge() == false && P2.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 10;
  trialcontrol();
  }
else if (P0.fallingEdge() && P1.fallingEdge() && P3.fallingEdge() && P2.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 11;
  trialcontrol();
  }
else if (P2.fallingEdge() && P3.fallingEdge() && P0.fallingEdge() == false && P1.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 12;
  trialcontrol();
  }
else if (P0.fallingEdge() && P2.fallingEdge() && P3.fallingEdge() && P1.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 13;
  trialcontrol();
  }
else if (P1.fallingEdge() && P2.fallingEdge() && P3.fallingEdge() && P0.fallingEdge() == false && P4.fallingEdge() == false){
  TRIAL = 14;
  trialcontrol();
  }
else if (P0.fallingEdge() && P1.fallingEdge() && P2.fallingEdge() && P3.fallingEdge() && P4.fallingEdge() == false){
  TRIAL = 15;
  trialcontrol();
  }
else if (P4.fallingEdge() && P0.fallingEdge() == false && P1.fallingEdge() == false && P2.fallingEdge() == false && P3.fallingEdge() == false){
  TRIAL = 16;
  trialcontrol();
  }
else if (P4.fallingEdge() && P2.fallingEdge() && P0.fallingEdge() == false && P1.fallingEdge() == false && P3.fallingEdge() == false){
  Serial2.write(17);
  //delay(8);
  startShow(0);
  }

else if (P4.fallingEdge() && P3.fallingEdge() && P0.fallingEdge() == false && P1.fallingEdge() == false && P2.fallingEdge() == false){
  
  report();
  }

delay(10);


/*
//SERIAL trial monitor
Serial.begin(9600);
Serial.print(TRIAL);
Serial.print("\t");
delay(1000);
*/

//Rotation Time report and light control from other Teensy
if(Tranny.receiveData()){
 
  //showType = rpmcData.tNUM;
  ROTtime = rpmcData.rotsend;
  RealPos = rpmcData.possend;
  

  //strip.show();
  //startShow(showType);

  //delay(10);
}






}

void trialcontrol(){
  startShow(TRIAL);
  Serial2.write(TRIAL);
  Serial2.flush(); //wait for serial data in buffer to transmit
  //delay(5);
  
  
}


void report(){
  Keyboard.print("P_");
  Keyboard.print(RealPos);
  Keyboard.print(" ");
  Keyboard.print("T_");
  Keyboard.print( (int)(ROTtime / 1000L));         // divide by 1000 to convert micros to millis - then cast to an integer
      Keyboard.print(".");                             // print decimal point
      fractional = (int)(ROTtime % 1000L);
      if (fractional == 0)
        Keyboard.print("000");      // add three zero's
      else if (fractional < 10)    // if fractional < 10 the 0 is ignored giving a wrong time, so add the zeros
        Keyboard.print("00");       // add two zeros
      else if (fractional < 100)
        Keyboard.print("0");        // add one zero
        Keyboard.println(fractional);  // print fractional part of time

  //LIVE MONITOR
  Serial.begin(9600);
  Serial.print(ROTtime);
  Serial.print("\t");
  Serial.end();      
   ROTtime = 999999; //ErrorCode 999999
   RealPos = 888888; //ErrorCode 888888
}



 
  void LightsOFF(){
  startShow(0);
  TRIAL = 0;
  
 }



//NeoPixel Lights
void startShow(int i) {
  switch (i) {
    case 0: strip.setPixelColor(0, 0, 0, 0);    // Black/off
      strip.setPixelColor(1, 0, 0, 0);
      strip.show();
      break;
    case 1: strip.setPixelColor(0, 5, 5, 255); //blue
      strip.setPixelColor(1, 127, 5, 255); // magenta
      strip.show();
      break;
    case 2: strip.setPixelColor(0, 5, 5, 255); //blue
      strip.setPixelColor(1, 127, 5, 255); // magenta
      strip.show();
      break;
    case 3: strip.setPixelColor(0, 127, 5, 255); // magenta
      strip.setPixelColor(1, 5, 5, 255); //blue
      strip.show();
      break;
    case 4: strip.setPixelColor(0, 127, 5, 255); // magenta
      strip.setPixelColor(1, 5, 5, 255); //blue
      strip.show();
      break;
    case 5: strip.setPixelColor(0, 127, 5, 255); // magenta
      strip.setPixelColor(1, 5, 5, 255); //blue
      strip.show();
      break;
    case 6: strip.setPixelColor(0, 127, 5, 255); // magenta
      strip.setPixelColor(1, 5, 5, 255); //blue
      strip.show();
      break;
    case 7: strip.setPixelColor(0, 5, 5, 255); //blue
      strip.setPixelColor(1, 127, 5, 255); // magenta
      strip.show();
      break;
    case 8: strip.setPixelColor(0, 5, 5, 255); //blue
      strip.setPixelColor(1, 127, 5, 255); // magenta
      strip.show();
      break;
    case 9: strip.setPixelColor(0, 2, 255, 33); // green
      strip.setPixelColor(1, 127, 255, 0); // yellow
      strip.show();
      break;
    case 10: strip.setPixelColor(0, 2, 255, 33); // green
      strip.setPixelColor(1, 127, 255, 0); // yellow
      strip.show();
      break;
    case 11: strip.setPixelColor(0, 127, 255, 0); // yellow
      strip.setPixelColor(1, 2, 255, 33); // green
      strip.show();
      break;
    case 12: strip.setPixelColor(0, 127, 255, 0); // yellow
      strip.setPixelColor(1, 2, 255, 33); // green
      strip.show();
      break;
    case 13: strip.setPixelColor(0, 127, 255, 0); // yellow
      strip.setPixelColor(1, 2, 255, 33); // green
      strip.show();
      break;
    case 14: strip.setPixelColor(0, 127, 255, 0); // yellow
      strip.setPixelColor(1, 2, 255, 33); // green
      strip.show();
      break;
    case 15: strip.setPixelColor(0, 2, 255, 33); // green
      strip.setPixelColor(1, 127, 255, 0); // yellow
      strip.show();
      break;
    case 16: strip.setPixelColor(0, 2, 255, 33); // green
      strip.setPixelColor(1, 127, 255, 0); // yellow
      strip.show();
      break;
     case 17: strip.setPixelColor(1, 0, 0, 0);    // Black/off
      strip.setPixelColor(0, 0, 0, 0);
      strip.show();
      break;
    case 18: strip.setPixelColor(1, 255, 0, 0); //red
      strip.setPixelColor(0, 255, 0, 0); //red
      strip.show();
      break;
  }
}
 
 


