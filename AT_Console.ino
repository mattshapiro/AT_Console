/**
*  @filename   :   SMS.cpp
*  @brief      :   SIM7600CE 4G HAT demo
*  @author     :   Kaloha from Waveshare
*
*  Copyright (C) Waveshare     April 27 2018
*  http://www.waveshare.com / http://www.waveshare.net
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documnetation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to  whom the Software is
* furished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "Waveshare_SIM7600.h"
#include "AnalogQWERTY.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define STM32 // board def (there has to be a better way, right Arduino IDE?)
#define DISPLAY_OLED_SSD1306_I2C
//#define DISPLAY_LCD_SPI

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#ifdef STM32
#define NUM_KEYPADS 3
int KEYBOARD_PINS[] = { PA0, PA1, PA2 };
#define POWERKEY PA15
#define LEDPIN PC13
#endif

#ifdef ATMEGA328
#define NUM_KEYPADS 3
int KEYBOARD_PINS[] = { A0, A1, A2 };
#define POWERKEY 2
#define LEDPIN 13
#endif

char phone_number[] = "**********";      //********** change it to the phone number you want to call
char text_message[] = "test";      //

AnalogQWERTY keyboard;
String command, response;

bool refresh;

void setup() {

  // general setup
  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT); // LED connect to pin PC13
  command = String("");
  response = String(F("error"));
  refresh = true; 

  // display init
   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
  }

  splash();

  // keyboard init
  keyboard = AnalogQWERTY();
  keyboard.init(&KEYBOARD_PINS[0], NUM_KEYPADS);

  delay(100);
  

  // LTE module init
  sim7600.PowerOn(POWERKEY);
  sim7600.Initialize(5000);

  Serial.println("initialized");
}


void loop() {
  blink();
  
  char key = keyboard.getKeyPress();
  if(key != NULL) {
    refresh = true;
    if(key == DELETE_KEY) {
      // delete pressed
      command.remove(command.length()-1);
    } else if (key == RETURN_KEY) {
      // return pressed
      int value = sim7600.sendATcommand(command.c_str(), "OK", 3000);
      if(value != 0) {
        response = "success";
      } else {
        response = "error";
      }
    } else {
      Serial.print(key);
      command += key;
    }
  }

  paint();  
}

void paint() {
  String temp = String(F(">"));
  if(refresh) {
    // Clear the buffer
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    temp += command;
    display.println(temp);
    display.setTextSize(1);
    display.println(response);
    // Show the display buffer on the screen. You MUST call display() after
    // drawing commands to make them visible on screen!
    display.display();
    refresh = false;
  }
}

void blink() {
  digitalWrite(PC13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for 100mS
  digitalWrite(PC13, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
  //Serial.println("blinky");
}

void splash()
{
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("Texto v0"));
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
}