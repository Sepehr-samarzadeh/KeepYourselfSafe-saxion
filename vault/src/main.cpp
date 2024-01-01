#include <Arduino.h>
#include "binary.h"
#include "time.h"
//TODO: all globals in the struct

//shift register pins
const int RCLK = 9; //RCLK pin number 12 is labeled as "RCLK" or "ST_CP" (Storage Register Clock Input). This pin is used to control when the data stored in the shift register is transferred to the storage register (output latch)  After shifting the desired data into the shift register, a pulse on this pin will update the parallel outputs, allowing the data to appear simultaneously on all the output pins. ST_CP
const int SRCLK = 8; //SRCLK This pin is responsible for shifting the data into the shift register, Connect this pin to the clock signal source (e.g., Arduino digital output) to control the shifting of data into the shift registe  Each pulse on this pin shifts the data by one bit. SH_CP
const int SER = 11; //SER  This is the input for the data that will be shifted into the shift register. cooect to the digital pin of arduino DATA INPUT DS_PIN


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



//functions:
//TODO:we need to find a way to pass data
void shiftOutData(byte data) {
    digitalWrite(RCLK, LOW);
    shiftOut(SER, SRCLK, MSBFIRST, data);
    digitalWrite(RCLK, HIGH);
}

//loop in 3 7-segment
void displayDigit(int digit, int displayCount, int delayTime) {
    for (int displayNumber = 0; displayNumber < displayCount; ++displayNumber) {
        //display the current number on display
        shiftOutData(segmentData[digit]);
        delay(delayTime);
    }
}



void setup() {
    pinMode(SER, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(SRCLK, OUTPUT);
}

void loop() {
    for (int digit = 0; digit < 10; ++digit) {
        displayDigit(digit, 3, 1000);
    }

}