#include <Arduino.h>
#include "binary.h"
#include "time.h"
//TODO: all globals in the struct

//shift register pins:
const int RCLK = 9; //RCLK pin number 12 is labeled as "RCLK" or "ST_CP" (Storage Register Clock Input). This pin is used to control when the data stored in the shift register is transferred to the storage register (output latch)  After shifting the desired data into the shift register, a pulse on this pin will update the parallel outputs, allowing the data to appear simultaneously on all the output pins. ST_CP
const int SRCLK = 8; //SRCLK This pin is responsible for shifting the data into the shift register, Connect this pin to the clock signal source (e.g., Arduino digital output) to control the shifting of data into the shift registe  Each pulse on this pin shifts the data by one bit. SH_CP
const int SER = 11; //SER  This is the input for the data that will be shifted into the shift register. cooect to the digital pin of arduino DATA INPUT DS_PIN

//BCD pins:
const int BCD_PinA = 5;  // connect to A2 of SN74LS47
const int BCD_PinB = 6;  // connect to A1 of SN74LS47
const int BCD_PinC = 7;  // connect to A0 of SN74LS47
const int BCD_PinLT = 8; // connect to LT of SN74LS47


//binary values for each digit
//FIXME:the binary reperesentation on 7-segment Display is not correct!
byte segmentData[] = {
        B00000011,  // 0
        B10011111,  // 1
        B00100101,  // 2
        B00001101,  // 3
        B10011001,  // 4
        B01001001,  // 5
        B01000001,  // 6
        B00011111,  // 7
        B00000001,  // 8
        B00001001   // 9
};

//binary values for each digit (BCD input for SN74LS47)
byte bcdData[] = {
        B0000,  // 0
        B0001,  // 1
        B0010,  // 2
        B0011,  // 3
        B0100,  // 4
        B0101,  // 5
        B0110,  // 6
        B0111,  // 7
        B1000,  // 8
        B1001   // 9
};



//functions:
//TODO:we need to find a way to pass data
void shiftOutData(byte data) {
    digitalWrite(RCLK, LOW);
    shiftOut(SER, SRCLK, MSBFIRST, data);
    digitalWrite(RCLK, HIGH);
}

void setBCDInputs(int value) {
    // set BCD inputs for decoder based on the decimal value
    ///fixme:on the 4bit switch we only need to work with 3 switch the 4th one is one-off so we use the bitwise operator to findout the state of the each witch key with &
    digitalWrite(BCD_PinA, (value & B0001) ? HIGH : LOW); // we need only the rightmost bit so we use &bitwise operator to get the last bit from the right if its 1 we have if isnt we got 0;
    digitalWrite(BCD_PinB, (value & B0010) ? HIGH : LOW);
    digitalWrite(BCD_PinC, (value & B0100) ? HIGH : LOW);
    digitalWrite(BCD_PinLT, LOW);
    digitalWrite(BCD_PinLT, HIGH);  // Pulse the latch pin
}

//loop in 3 7-segment
void displayDigit(int digit, int displayCount, int delayTime) {
    for (int displayNumber = 0; displayNumber < displayCount; ++displayNumber) {
        // Set BCD inputs for SN74LS47
        setBCDInputs(bcdData[digit]);
        //display the current number on display
        shiftOutData(segmentData[digit]);
        delay(delayTime);
    }
}

void setup() {
    pinMode(SER, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    pinMode(BCD_PinA, OUTPUT);
    pinMode(BCD_PinB, OUTPUT);
    pinMode(BCD_PinC, OUTPUT);
    pinMode(BCD_PinLT, OUTPUT);
}
 //FIXME: the digit from rotary encoder goes to the bcd and then we get that and give it to our function
void loop() {
    for (int digit = 0; digit < 10; ++digit) {
        displayDigit(digit, 3, 1000);
    }

}