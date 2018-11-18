#include <Arduino.h>

#include "MCP7940.h"
#include "LowPower.h"

// Constant
const int BUTTON_PIN_ENTER = 2;
const int BUTTON_PIN_CH = 3;

const uint8_t  SPRINTF_BUFFER_SIZE = 32;
const int SERIAL_ACTIVE = 1;

//Pin connected to ST_CP of 74HC595
const int latchPin = 8;
//Pin connected to SH_CP of 74HC595
const int clockPin = 12;
//Pin connected to DS of 74HC595
const int dataPin = 11;

// Variables
MCP7940_Class MCP7940;
char inputBuffer[SPRINTF_BUFFER_SIZE];
int setTimeState = 0;
int setHour = 0;
int setMinutes = 0;

int countSec = 0;
int countSec2 = 0;
int countSec3 = 0;
int countSec4 = 0;

static uint8_t secs;

// 74HC595 Control
void shiftOut(int myDataPin, int myClockPin, byte myDataOut)
{
    // This shifts 8 bits out MSB first,
    //on the rising edge of the clock,
    //clock idles low

    //internal function setup
    int i=0;
    int pinState;
    pinMode(myClockPin, OUTPUT);
    pinMode(myDataPin, OUTPUT);

    //clear everything out just in case to
    //prepare shift register for bit shifting
    digitalWrite(myDataPin, 0);
    digitalWrite(myClockPin, 0);

    //for each bit in the byte myDataOut�
    //NOTICE THAT WE ARE COUNTING DOWN in our for loop
    //This means that %00000001 or "1" will go through such
    //that it will be pin Q0 that lights.
    for (i=7; i>=0; i--)
    {
        digitalWrite(myClockPin, 0);

        //if the value passed to myDataOut and a bitmask result
        // true then... so if we are at i=6 and our value is
        // %11010100 it would the code compares it to %01000000
        // and proceeds to set pinState to 1.
        if ( myDataOut & (1<<i) )
        {
            pinState= 1;
        }
        else
        {
            pinState= 0;
        }

        //Sets the pin to HIGH or LOW depending on pinState
        digitalWrite(myDataPin, pinState);
        //register shifts bits on upstroke of clock pin
        digitalWrite(myClockPin, 1);
        //zero the data pin after shift to prevent bleed through
        digitalWrite(myDataPin, 0);
    }

    //stop shifting
    digitalWrite(myClockPin, 0);
}

int displayThrough74HC595(int h1, int h2, int m1, int m2)
{
    byte numbersToDisplay[] = {
        B11000000,  //  0
        B11111001,  //  1
        B10100100,  //  2
        B10110000,  //  3
        B10011001,  //  4
        B10010010,  //  5
        B10000010,  //  6
        B11111000,  //  7
        B10000000,  //  8
        B10011000,  //  9
        B10001001,  //H
        B11000000,  //O
        B11000111,  //L
        B10001000   //A
    };

    // 74HC595
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, 0);
    //move 'em out
    shiftOut(dataPin, clockPin, numbersToDisplay[m2]);
    shiftOut(dataPin, clockPin, numbersToDisplay[m1]);
    shiftOut(dataPin, clockPin, numbersToDisplay[h2]);
    shiftOut(dataPin, clockPin, numbersToDisplay[h1]);

    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(latchPin, 1);
    delay(100);

    return 0;
}

// Find the first digit
int firstDigit(int n)
{
  if(n < 10)
  return 0;
    // Remove last digit from number
    // till only one digit is left
    while (n >= 10)
        n /= 10;

    // return the first digit
    return n;
}

// Find the last digit
int lastDigit(int n)
{
    // return the last digit
    return (n % 10);
}


void setup()
{
    // Led output
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(BUTTON_PIN_ENTER, INPUT);
    pinMode(BUTTON_PIN_CH, INPUT);

    // Serial output
    if(SERIAL_ACTIVE == 1)
        Serial.begin(9600);

    //74HC595 set pins to output because they are addressed in the main loop
    pinMode(latchPin, OUTPUT);
    displayThrough74HC595(8,8,8,8);

    // RTC
    while (!MCP7940.begin())
    {
        if(SERIAL_ACTIVE == 1)
            Serial.println(F("Unable to find MCP7940M. Checking again in 3s."));
        delay(3000);
    }

    if(SERIAL_ACTIVE == 1)
        Serial.println(F("MCP7940 initialized."));

    while (!MCP7940.deviceStatus())
    {
        if(SERIAL_ACTIVE == 1)
            Serial.println(F("Oscillator is off, turning it on."));

        bool deviceStatus = MCP7940.deviceStart();
        if (!deviceStatus)
        {
            if(SERIAL_ACTIVE == 1)
                Serial.println(F("Oscillator did not start, trying again."));
            delay(1000);
        }
    }

    //MCP7940.adjust();   // Adjust time with the compilation time
    MCP7940.adjust(DateTime(2018,11,7,21,39,10));   //Set a specific time

    // Optional msg hello
    // displayThrough74HC595(10, 11, 12, 13);

    delay(2000);
}

void loop()
{
    // [Optional counter instead of hh:mm]
    countSec = countSec + 1;
    if(countSec >= 10)
    {
      countSec2 = countSec2 + 1;
      countSec = 0;
    }

    if(countSec2 >= 10)
    {
      countSec3 = countSec3 + 1;
      countSec2 = 0;
    }

    if(countSec3 >= 10)
    {
      countSec4 = countSec4 + 1;
      countSec3 = 0;
    }

    if(countSec4 >= 10)
    {
      countSec4 = 0;
      countSec3 = 0;
      countSec2 = 0;
      countSec = 0;
    }

    if(setTimeState < 2)
    {
        if(digitalRead(BUTTON_PIN_ENTER) == HIGH)
            setTimeState = setTimeState + 1;

        if(digitalRead(BUTTON_PIN_CH) == HIGH)
        {
            if(setTimeState == 0)
            {
                setHour = setHour + 1;
            }
            else if(setTimeState == 1)
            {
                setMinutes = setMinutes + 1;
            }
        }

        displayThrough74HC595(firstDigit(setHour), lastDigit(setHour), firstDigit(setMinutes), lastDigit(setMinutes));

        delay(250);

        if(setTimeState == 2)
        {
            MCP7940.adjust(DateTime(2018,11,11,setHour,setMinutes,0));   //Set a specific time
        }
    }
    else
    {
        DateTime now = MCP7940.now();

        if (secs != now.second())
        {
            sprintf(inputBuffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(),
                    now.month(), now.day(), now.hour(), now.minute(), now.second());

            if(SERIAL_ACTIVE == 1)
            Serial.println(inputBuffer);

            secs = now.second();

            // Toogle the led when reading time
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);

            displayThrough74HC595(firstDigit(now.hour()), lastDigit(now.hour()), firstDigit(now.minute()), lastDigit(now.minute()));

            //  displayThrough74HC595(countSec4, countSec3, countSec2, countSec);// Counter instead of hh:mm

        }
    }

    delay(100);
}
