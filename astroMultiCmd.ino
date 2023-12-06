#include <DS3231.h>
#include <FastLED.h>
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <Arduino.h>
#include <hp_BH1750.h>


/**        
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

uint8_t record[7]; // save record
uint8_t buf[255];

//Button Setup
const int buttonPin = 4; // Define the pin for the button
int buttonState = 0; // Variable to store the button state
int lastButtonState = 0; // Variable to store the previous button state

//LED Setup
#define DATA_PIN 6 
#define NUM_LEDS 55
CRGB leds[NUM_LEDS];

#define RED_LED 12



int isOn, isListening, setting;
long triggerTime = -1;
byte seconds;

#define astroCalled (50) //astro
#define onRecord    (51) //start
#define offRecord   (52) //end
#define low         (53) //low
#define medium      (54) //mid
#define high        (55) //high

#define astroCalled1 (60)



//Light Sensor Setup
hp_BH1750 BH1750;
float lux;

//Time Clock Setup
DS3231  rtc(SDA, SCL);

/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf     --> command length
           len     --> number of parameters
*/
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      //Serial.print("[");
      //Serial.print(buf[i], HEX);
      //Serial.print("]");
    }
  }
}

/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf  -->  VR module return value when voice is recognized.
             buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
             buf[1]  -->  number of record which is recognized. 
             buf[2]  -->  Recognizer index(position) value of the recognized record.
             buf[3]  -->  Signature length
             buf[4]~buf[n] --> Signature
*/
void printVR(uint8_t *buf)
{
  //Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  //Serial.print(buf[2], DEC);
  //Serial.print("\t\t");

  if(buf[0] == 0xFF){
    //Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    //Serial.print("UG ");
    //Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    //Serial.print("SG ");
    //Serial.print(buf[0], DEC);
  }
  //Serial.print("\t");

  //Serial.print(buf[1], DEC);
  //Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    //Serial.print("NONE");
  }
  //Serial.println("\r\n");
}

//sets all pixels to specified RGB value
void ledOn(int r, int g, int b) {
  for(int i=0; i<NUM_LEDS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // set LED brightness based on setting value, blue value based on hr
    leds[i] = CRGB(r, g, b);
    FastLED.show();
    // Send the updated pixel colors to the hardware.
  }
}

//turns off all LEDs
void ledOff() {
  ledOn(0,0,0);
}

/*
[0]: before 6pm: 1:1:1
[1]: from 6-9pm: 1:.8:.5
[2]: after 9pm: 1:.3:0
*/
//
double color[3][3] = { {1, 1, 1}, {1, 1, .4}, {.3, 1, 0} }; //color[i] is the color ratio for the respective time block
int findTimeBlock(int hr) {
  if (hr >= 6 && hr < 18) return 0;
  else if (hr >= 18 && hr < 21) return 1;
  else return 2;
}

//finds correct setting based on lux (the brighter the room is, the lower the setting)
int findSetting(double luxVal) {
  if (0 <= luxVal && luxVal <70) return 1;
  else if (luxVal >= 70 && luxVal < 140) return 2;
  else return 3;
}

//finds scale factor based on setting (larger scale factor (brighter) for higher setting)
double findFactor(int settingVal) {
  switch (settingVal) {
    case 1:
      return 85;
    case 2:
      return 170;
    case 3:
      return 255;
  }
  return 0;
}

void setup()
{
  /** initialize */
  myVR.begin(9600);
  
  Serial.begin(115200);
  //Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
    
  if(myVR.clear() == 0){
    //Serial.println("Recognizer cleared.");
  }else{
    //Serial.println("Not find VoiceRecognitionModule.");
    //Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  

  //Button Setup
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);

  //initializing states
  isOn = 0;
  isListening = 0;
  setting = -1;

  //Light Sensor Setup
  BH1750.begin(BH1750_TO_GROUND);

  //Time Clock Setup
  rtc.begin();
  // rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  // rtc.setTime(1, 19, 0);     // Set the time to 12:00:00 (24hr format)
  // rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014

  //LED Setup
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  record[0] = astroCalled;
  record[1] = onRecord;
  record[2] = offRecord;
  record[3] = low;
  record[4] = medium;
  record[5] = high;
  record[6] = astroCalled1;
  if (myVR.load(record, 7) >= 0) {
    Serial.println("loaded");
  }
}

void loop()
{
  if ((millis() - triggerTime) > 20000 && isListening == 1) {
    isListening = 0;
    Serial.println("no keyword said, listening state is off");
    digitalWrite(RED_LED, LOW);
  }

  int ret;
  ret = myVR.recognize(buf, 50);
  if (ret>0) {
    Serial.print("i hear something!  ");
    if (buf[1] == astroCalled || buf[1] == astroCalled1) {
      isListening = 1;
      triggerTime = millis();
      Serial.println("astro called, listening state is on");
      digitalWrite(RED_LED, HIGH);
    }
    else {
      if (isListening == 1){
        switch(buf[1]) {
          // case astroCalled:
          //   Serial.println("astro called");
          //   break;
          // case astroCalled1:
          //   Serial.println("astro1 called");
          //   break;
          case onRecord:
            /** turn on LED */
            Serial.println("on called");
            isOn = 1;
            setting = -1;
            isListening = 0;
            digitalWrite(RED_LED, LOW);
            break;
          case low:
            Serial.println("low called");
            setting = 1;
            isListening = 0;
            digitalWrite(RED_LED, LOW);
            break;
          case medium:
            Serial.println("mid called");
            setting = 2;
            isListening = 0;
            digitalWrite(RED_LED, LOW);
            break;
          case high: 
            Serial.println("high called");
            setting = 3;
            isListening = 0;
            digitalWrite(RED_LED, LOW);
            break;
          case offRecord:
            /** turn off LED*/
            Serial.println("off called");
            isOn = 0;
            ledOff();
            setting = -1;
            isListening = 0;
            digitalWrite(RED_LED, LOW);
            break;
          default:
            Serial.println("Record function undefined");
            break;
        }
      }
    }
    printVR(buf);
  }

  buttonState = digitalRead(buttonPin);
  if (isOn == 1) {
    //cycling through settings with each button press
    if ((buttonState != lastButtonState) && (buttonState == LOW)) {
      setting++;
      //Serial.print("Button pressed, setting is now ");
      //Serial.println(setting);
    }

    if (setting > 3) {
      //turn off lamp
      isOn = 0;
      setting = -1;
      ledOff();
      //Serial.println("Turning off");
    }
    //if lamp remains on after button press, output to LED
    else {
       //Serial.print("before tuning: setting is now ");
      //Serial.println(setting);
      //get time reading
      Time currentTime = rtc.getTime();
      //int hr = currentTime.hour;
      int hr = currentTime.hour;
      int index = findTimeBlock(hr);
      // Serial.println(hr);
      //index = 0; //change number to be certain time block

      //if just turned on
      if (setting == 0|| setting == -1) {
        //get correct setting from lux reading
        BH1750.start();  
        lux = BH1750.getLux();
        setting = findSetting(lux);
        //Serial.println(setting);
      }

      //get scale factor by setting
      int factor = findFactor(setting);
      //turn on LEDs to correct color + brightness
      ledOn(color[index][0]*factor, color[index][1]*factor, color[index][2]*factor);
    }
  }
  else {
    //turn off LEDs
    ledOff();
    setting = -1;
    //if button is pressed, turn on
    if ((buttonState != lastButtonState) && (buttonState == LOW)) {
      //Serial.println("Button Pressed -> turning on");
      isOn = 1;
      setting = -1;
    }
  }
  lastButtonState = buttonState;
}



