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

//#include <BLE.h>
#include <KeyboardBLE.h>
#include <Adafruit_NeoPixel.h>

//BLEDevice central = BLE.central();

#define KEYSCANTIME 5  // delay between key scans for debounce
#define DEBOUNCECOUNT 4 //  number of keyscan delays for debounce

#define NUMKEYS 8

// GPIOs that sense key closures to GND
#define EDITKEY 10
#define OPTKEY 15
#define UPKEY 14
#define RIGHTKEY 13
#define DOWNKEY 12
#define LEFTKEY 11
#define SHIFTKEY 4
#define PLAYKEY 5

int8_t keyGPIO [8]= {EDITKEY,OPTKEY,UPKEY,RIGHTKEY,DOWNKEY,LEFTKEY,SHIFTKEY,PLAYKEY}; // key index 0-7 to key GPIO mapping

#define LEDPIN 3
#define NUMPIXELS 1 // 
#define LEDFLASH 100
#define LED_RED 0x030000  // only using 2 bits to keep LEDs from getting too bright and to save power
#define LED_GREEN 0x000300
#define LED_BLUE 0x000003
#define LED_OFF 0
// battery level colors from red to green
#define BATTCOLORS 4
int32_t battcolors[]={LED_RED,0x020000+0x000100,0x010000+0x000200,LED_GREEN};

Adafruit_NeoPixel LED(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

#define BATTERYVOLTAGEINTERNAL 29  // internal connection on Pico W 2
#define BATTERYVOLTAGE 26  // external resistor divider connection
// bat voltage ranges for LiPo
//#define BATOK 560  // approx 3.6v - bat voltage read thru external 2:1 divider
//#define BATLOW 500 // approx 3.2v
// bat voltage ranges for 3x AAA batteries
#define BATOK 580  // approx 3.75v - bat voltage read thru external 2:1 divider
#define BATLOW 450 // approx 3 v
#define BATTSAMPLE 1000 // timer period for battery sampling. don't want to update the RGB LED too often because it could cause issues with the BLE signal

void BLE_flash(void) {
  LED.setPixelColor(0,LED_BLUE); 
  LED.show();
  delay(LEDFLASH);
  LED.setPixelColor(0,LED_OFF); 
  LED.show();
  delay(LEDFLASH);
}

void showbatterylevel(void) { 
  int16_t batvoltage=analogRead(BATTERYVOLTAGE);
//  Serial.printf("bat = %d\n",batvoltage);
  batvoltage=constrain(batvoltage,BATLOW,BATOK);
  LED.setPixelColor(0,battcolors[map(batvoltage,BATLOW,BATOK,0,BATTCOLORS-1)]); 
  LED.show();
}

void setup() {
  Serial.begin(115200);
  for (uint8_t i=0;i<NUMKEYS;++i) pinMode(keyGPIO[i],INPUT_PULLUP); 

  LED.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  showbatterylevel();
  KeyboardBLE.begin();
//  while (!central.connected()) BLE_flash(); // so far I can't figure out how to detect BLE connection status
}

void loop() {
  static uint16_t keymap=0;
  static uint16_t keycnt=0;
  static uint32_t batt_timer;
  static int16_t scancnt;
  bool keystate;


  for (uint8_t i=0;i<NUMKEYS;++i)  {
    keystate=!digitalRead(keyGPIO[i]);  // read key 
    if ((bool)(keymap & (1<<i)) != keystate) { // if its different than last scan
      if (keycnt > DEBOUNCECOUNT ) {  // debounce
        if (keystate) keymap|=1 <<i;  // debounced, update state
        else keymap &= 0xfe << i;

        switch (i) {  // process key up and down
          case 0:  // edit key
            if (keystate) KeyboardBLE.press('X');
            else KeyboardBLE.release('X');
            break;
          case 1: // OPT key
            if (keystate) KeyboardBLE.press('Z');
            else KeyboardBLE.release('Z');
            break;
          case 2: // UP key
            if (keystate) KeyboardBLE.press(KEY_UP_ARROW);
            else KeyboardBLE.release(KEY_UP_ARROW);
            break;
          case 3: // RIGHT key
            if (keystate) KeyboardBLE.press(KEY_RIGHT_ARROW);
            else KeyboardBLE.release(KEY_RIGHT_ARROW);
            break;
          case 4: // DOWN key
            if (keystate) KeyboardBLE.press(KEY_DOWN_ARROW);
            else KeyboardBLE.release(KEY_DOWN_ARROW);
            break;
          case 5: // LEFT key
            if (keystate) KeyboardBLE.press(KEY_LEFT_ARROW);
            else KeyboardBLE.release(KEY_LEFT_ARROW);
            break;
          case 6: // SHIFT key
            if (keystate) KeyboardBLE.press(KEY_RIGHT_SHIFT);  
            else KeyboardBLE.release(KEY_RIGHT_SHIFT);
            break;
          case 7:  // play key
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

  if ((millis()-batt_timer) > BATTSAMPLE) {
    batt_timer=millis();
    showbatterylevel();
  }
}