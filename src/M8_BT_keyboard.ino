/*
// Copyright (c) 2026, Rich Heslip
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
/*

BLE keyboard using Raspberry Pi Pico 2 for the M8 iOS app

Sept 15/2026 - first drop of the code for the prototype HW. 

based on example code by Tom Igoe:

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/KeyboardSerial
*/

#include <KeyboardBLE.h>

#define KEYSCANTIME 5  // delay between key scans for debounce
#define DEBOUNCECOUNT 4 //  number of keyscan delays for debounce

#define NUMKEYS 9

// GPIOs that sense key closures to GND
#define EDITKEY 10
#define OPTKEY 11
#define UPKEY 12
#define RIGHTKEY 13
#define DOWNKEY 14
#define LEFTKEY 15
#define SHIFTKEYL 16
#define SHIFTKEYR 17
#define PLAYKEY 18

#define REDLED 9
#define GREENLED 8
#define BATTERYVOLTAGE 28
#define BATOK 560  // approx 3.6v - bat voltage read thru external 2:1 divider
#define BATLOW 500 // approx 3.2v

void setup() {
  Serial.begin(115200);
  for (uint8_t i=0;i<NUMKEYS;++i) pinMode(EDITKEY+i, INPUT_PULLUP); 

  pinMode(REDLED,OUTPUT);
  pinMode(GREENLED,OUTPUT);  

  KeyboardBLE.begin();
}

void loop() {
  static uint16_t keymap=0;
  static uint16_t keycnt=0;
  static uint32_t keytimer;
  static int16_t scancnt;
  bool keystate;


  for (uint8_t i=0;i<NUMKEYS;++i)  {
    keystate=!digitalRead(EDITKEY+i);
    if ((bool)(keymap & (1<<i)) != keystate) {
      if (keycnt > DEBOUNCECOUNT ) {
        if (keystate) keymap|=1 <<i;
        else keymap &= 0xfe << i;
  //      Serial.printf("key %d map %x state %d\n",i,keymap,keystate);
        switch (i) {
          case 0:  // edit key
            if (keystate) KeyboardBLE.press('X');
            else KeyboardBLE.release('X');
            break;
          case 1: // OPT key
            if (keystate) KeyboardBLE.press('Z');
            else KeyboardBLE.release('Z');
            break;
          case 2: // UP key
            if (keystate) KeyboardBLE.press('W');
            else KeyboardBLE.release('W');
            break;
          case 3: // RIGHT key
            if (keystate) KeyboardBLE.press('D');
            else KeyboardBLE.release('D');
            break;
          case 4: // DOWN key
            if (keystate) KeyboardBLE.press('S');
            else KeyboardBLE.release('S');
            break;
          case 5: // LEFT key
            if (keystate) KeyboardBLE.press('A');
            else KeyboardBLE.release('A');
            break;
          case 6: // left and right SHIFT key
          case 7:
            if (keystate) KeyboardBLE.press('Q');
            else KeyboardBLE.release('Q');
            break;
          case 8:  // play key
            if (keystate) KeyboardBLE.press(' ');
            else KeyboardBLE.release(' ');
            break;       
        }
        delay(10); // We need a short delay here, sending packets with less than 1ms leads to packet loss!
      }
      else {
        ++keycnt;
        delay(1); // key bounce delay
      }
    }   
  }

  // monitor battery voltage and show on R/G led
  int16_t batvoltage=analogRead(BATTERYVOLTAGE);
  if (batvoltage > BATOK) {
    digitalWrite (REDLED,0);
    digitalWrite (GREENLED,1);
  }
  else if (batvoltage < BATLOW) {
    digitalWrite (REDLED,1);
    digitalWrite (GREENLED,0);
  } 
  else {
    digitalWrite (REDLED,1);  // orange for kinda low
    digitalWrite (GREENLED,1);
  } 
 // Serial.printf("bat %d\n",batvoltage);
 // delay(100);
}