// testshapes demo for RGBmatrixPanel library.
// Demonstrates the drawing abilities of the RGBmatrixPanel library.
// For 32x64 RGB LED matrix.

// WILL NOT FIT on ARDUINO UNO -- requires a Mega, M0 or M4 board
#include <Arduino.h>
#include <RGBmatrixPanel.h>
#include <SPI.h>

// Most of the signal pins are configurable, but the CLK pin has some
// special constraints.  On 8-bit AVR boards it must be on PORTB...
// Pin 8 works on the Arduino Uno & compatibles (e.g. Adafruit Metro),
// Pin 11 works on the Arduino Mega.  On 32-bit SAMD boards it must be
// on the same PORT as the RGB data pins (D2-D7)...
// Pin 8 works on the Adafruit Metro M0 or Arduino Zero,
// Pin A4 works on the Adafruit Metro M4 (if using the Adafruit RGB
// Matrix Shield, cut trace between CLK pads and run a wire to A4).

//#define CLK  8   // USE THIS ON ADAFRUIT METRO M0, etc.
//#define CLK A4 // USE THIS ON METRO M4 (not M0)
#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

#define upPin 2
#define downPin 3
#define resetPin 18

int resultCounter = 0;
int countDownSeconds = 0;
int timerOptionA = 600;
int timerOptionB = 1800;
int timerOptionC = 3600;

int sessionTimer = 0;

int stateMachine = 0;


RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);
// Input a value 0 to 24 to get a color value.
// The colours are a transition r - g - b - back to r.
uint16_t Wheel(byte WheelPos) {
  if(WheelPos < 8) {
   return matrix.Color333(7 - WheelPos, WheelPos, 0);
  } else if(WheelPos < 16) {
   WheelPos -= 8;
   return matrix.Color333(0, 7-WheelPos, WheelPos);
  } else {
   WheelPos -= 16;
   return matrix.Color333(WheelPos, 0, 7 - WheelPos);
  }
}

void clearResultScreen(){
  matrix.fillRect(0, 0, matrix.width(), 16, matrix.Color333(0, 0, 0));
}
void clearTimerScreen(){
  matrix.fillRect(0, 17, matrix.width(), 32, matrix.Color333(0,0,0));
}
void clearScreen(){
  matrix.fillRect(0,0, matrix.width(), matrix.height(), matrix.Color333(0,0,0));
}

void printTime(){
  if (countDownSeconds != 0){
      matrix.setCursor(0,17);
      matrix.setTextSize(2);
      matrix.setTextColor(matrix.Color333(0,0,7));
      matrix.print(String(countDownSeconds / 60));
      matrix.print(':');
      if (countDownSeconds % 60 < 10){
        matrix.print('0');
      }

      matrix.print(String(countDownSeconds % 60));
  }
  else {
      matrix.setCursor(0,17);
      matrix.setTextSize(2);
      matrix.setTextColor(matrix.Color333(0,0,7));
      matrix.print("Time");
      countDownSeconds = 0;
  }
  }



void addResult() {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 500){
      if(stateMachine == 2){
        resultCounter++;
        clearResultScreen();
        matrix.setTextColor(matrix.Color333(7,0,0));
        matrix.setCursor(0, 0);
        matrix.setTextSize(2);
        matrix.print(String(resultCounter));
      }
      else{
        countDownSeconds = timerOptionA;
        sessionTimer = timerOptionA;
        stateMachine = 1;
        clearScreen();
      }
    }
    last_interrupt_time = interrupt_time;
}
void removeResult(){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 500){
      if(stateMachine == 2){
        resultCounter--;
        clearResultScreen();
        matrix.setTextColor(matrix.Color333(7,0,0));
        matrix.setCursor(0, 0);
        matrix.setTextSize(2);
        matrix.print(String(resultCounter));
      }
      else{
        countDownSeconds = timerOptionB;
        sessionTimer = timerOptionB;
        stateMachine = 1;
        clearScreen();
      }
    }
    last_interrupt_time = interrupt_time;
}
void resetResults(){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 500){
      if ((stateMachine == 1) || (stateMachine == 2)){
        resultCounter = 0;
        countDownSeconds = sessionTimer;
        clearScreen();
        printTime();
        matrix.setTextColor(matrix.Color333(7,0,0));
        matrix.setCursor(0, 0);
        matrix.setTextSize(2);
        matrix.print(String(resultCounter));
        if(stateMachine == 2){
          stateMachine = 1;
        }
        else{
          stateMachine = 2;
        }
      }
      else{
        countDownSeconds = timerOptionC;
        sessionTimer = timerOptionC;
        stateMachine = 1;
        clearScreen();
      }
    }
    last_interrupt_time = interrupt_time;
}



void setup() {


  Serial.begin(9600);
  matrix.begin();
  Serial.print(stateMachine);

  pinMode(upPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(upPin), addResult, FALLING);
  attachInterrupt(digitalPinToInterrupt(downPin), removeResult, FALLING);
  attachInterrupt(digitalPinToInterrupt(resetPin), resetResults, FALLING);

    // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  matrix.setCursor(0,0);
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.setTextSize(1);
  matrix.print("Set timer");

  matrix.setCursor(0,17);
  matrix.setTextColor(matrix.Color333(0,7,7));
  matrix.print("A)");

  matrix.setCursor(21,17);
  matrix.setTextColor(matrix.Color333(7,0,7));
  matrix.print("B)");

  matrix.setCursor(42,17);
  matrix.setTextColor(matrix.Color333(7,7,0));
  matrix.print("C)");

  


 /*
  // draw a pixel in solid white
  matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
  delay(500);

  // fix the screen with green
  matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 7, 0));
  delay(500);

  // draw a box in yellow
  matrix.drawRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(7, 7, 0));
  delay(500);

  // draw an 'X' in red
  matrix.drawLine(0, 0, matrix.width()-1, matrix.height()-1, matrix.Color333(7, 0, 0));
  matrix.drawLine(matrix.width()-1, 0, 0, matrix.height()-1, matrix.Color333(7, 0, 0));
  delay(500);

  // draw a blue circle
  matrix.drawCircle(10, 10, 10, matrix.Color333(0, 0, 7));
  delay(500);

  // fill a violet circle
  matrix.fillCircle(40, 21, 10, matrix.Color333(7, 0, 7));
  delay(500);

  // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // draw some text!
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves

  matrix.setCursor(8, 0);    // start at top left, with 8 pixel of spacing
  uint8_t w = 0;
  char *str = "AdafruitIndustries";
  for (w=0; w<8; w++) {
    matrix.setTextColor(Wheel(w));
    matrix.print(str[w]);
  }
  matrix.setCursor(2, 8);    // next line
  for (w=8; w<18; w++) {
    matrix.setTextColor(Wheel(w));
    matrix.print(str[w]);
  }
  matrix.println();
  //matrix.setTextColor(matrix.Color333(4,4,4));
  //matrix.println("Industries");
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.println("LED MATRIX!");

  // print each letter with a rainbow color
  matrix.setTextColor(matrix.Color333(7,0,0));
  matrix.print('3');
  matrix.setTextColor(matrix.Color333(7,4,0));
  matrix.print('2');
  matrix.setTextColor(matrix.Color333(7,7,0));
  matrix.print('x');
  matrix.setTextColor(matrix.Color333(4,7,0));
  matrix.print('6');
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.print('4');
  matrix.setCursor(34, 24);
  matrix.setTextColor(matrix.Color333(0,7,7));
  matrix.print('*');
  matrix.setTextColor(matrix.Color333(0,4,7));
  matrix.print('R');
  matrix.setTextColor(matrix.Color333(0,0,7));
  matrix.print('G');
  matrix.setTextColor(matrix.Color333(4,0,7));
  matrix.print('B');
  matrix.setTextColor(matrix.Color333(7,0,4));
  matrix.print('*');

  // whew!*/
}

void loop() {
  if(stateMachine == 2){
    Serial.print(stateMachine);

    noInterrupts();
    clearTimerScreen();
    printTime();
    countDownSeconds--;
    interrupts();
    Serial.print("i am here");
    delay(1000);
  }
  else if(stateMachine == 1){
    Serial.print(stateMachine);
    clearScreen();
    matrix.setCursor(0,0);
    matrix.setTextColor(matrix.Color333(7,7,7));
    matrix.setTextSize(1);
    matrix.print("Press:");
    matrix.setCursor(0,17);
    matrix.print("Reset");
    while (stateMachine == 1)
    {
      delay(1000);
      Serial.print(stateMachine);
    }
    

  }
  Serial.print(stateMachine);
}


