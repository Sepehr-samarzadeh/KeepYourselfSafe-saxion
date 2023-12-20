#include <Arduino.h>

//shift register pins
const int latchPin = 9;
const int clockPin = 8;
const int dataPin = 10;

byte getSegmentDataForNumber(int number);
// function to shift out a byte to the shift register
void shiftOutByte(byte data) {
    digitalWrite(latchPin, LOW);  // start the transfer
    shiftOut(dataPin, clockPin, MSBFIRST, data);  // shift out the data
    digitalWrite(latchPin, HIGH);  // end the transfer
}

void setup() {
    // set shift register pins as output
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    //setup code...
}

void loop() {
    //display a number on the seven-segment display
    int numberToDisplay = 5;  //example for showing the number
    byte segmentData = getSegmentDataForNumber(numberToDisplay); //TODO:needs working
    shiftOutByte(segmentData);

}

//get segment data for a specific number
byte getSegmentDataForNumber(int number) {
    // TODO:logic to map a number to seven-segment display data
    // TODO: we need to find a mapping method
    // TODO:return the appropriate byte for the seven-segment display
    //simple mapping for numbers 0-9
    byte segmentData[] = {
            B11111100,  // 0
            B01100000,  // 1
            B11011010,  // 2
            B11110010,  // 3
            B01100110,  // 4
            B10110110,  // 5
            B10111110,  // 6
            B11100000,  // 7
            B11111110,  // 8
            B11110110   // 9
    };

    // TODO: make sure the number is in the a valid range
    if (number >= 0 && number <= 9) {
        return segmentData[number];
    } else {
        return 0;  //display nothing
    }
}





