#include <Arduino.h>
//rotary encoder
#define outputA 12
#define outputB 11
#define button 9
#define RCLK1 10
#define SER 6
#define RCLK2 7
#define SRCLK 8

int code[3];
int segmentData[10] = { 64, 121, 36, 48, 25, 18, 2, 120, 0, 24 };
int pressed;
const int bcdpin[4] = { 2, 3, 4, 5 };
int RotaryEncoder() {
    static unsigned long prev_time_of_change_in_A = 0;
    int static old = LOW;  // old value of A
    int b;
    int a;
    static int counter = 0;  // moved from global to  here
    a = digitalRead(outputA);

    if (a != old && millis() - prev_time_of_change_in_A > 50) {
        prev_time_of_change_in_A = millis();
        old = a;

        if (a == LOW) {              // if a has changed from HIGH to LOW
            b = digitalRead(outputB);  // read b
            if (b == 0) {              // if b is zero, then we're increasing
                counter++;
            } else {  // otherwisewe're decreasing
                counter--;
            }
            if (counter > 9) {
                counter = 0;
            }
            if (counter < 0) {
                counter = 9;
            }
        }
    }
    return counter;
}

int ButtonPressed() {
    static unsigned long previous_time = 0;
    static int previous_state = HIGH;
    unsigned long current_time = millis();
    int pressed;
    int buttonState = digitalRead(button);
    // if there's a change in state that happens 50ms after the last change, respond:
    if (buttonState != previous_state && current_time - previous_time > 50) {
        previous_state = buttonState;
        previous_time = current_time;

        if (buttonState == LOW) {  // if the button is now pressed, return 1
            return 1;
        }
    }
    return 0;
}
void shiftOutData1(int ShiftNum) {
    digitalWrite(RCLK1, LOW);
    shiftOut(SER, SRCLK, MSBFIRST, ShiftNum);
    digitalWrite(RCLK1, HIGH);
}
void shiftOutData2(int ShiftNum) {
    digitalWrite(RCLK2, LOW);
    shiftOut(SER, SRCLK, MSBFIRST, ShiftNum);
    digitalWrite(RCLK2, HIGH);
}
void decoder(int bcdNumber) {
    int bcdData[4] = { 0, 0, 0, 0 };
    for (int j = 0; j < 4; j++) {
        bcdData[j] = bcdNumber % 2;  //change the input data to the binary
        bcdNumber = bcdNumber / 2;   //change the input data to binary
    }
    for (int k = 0; k < 4; k++) {
        digitalWrite(bcdpin[k], bcdData[k]);
    }
}




void setup() {
    //rotary encoder
    pinMode(outputA, INPUT);
    pinMode(outputB, INPUT);
    pinMode(button, INPUT_PULLUP);
    pinMode(SER, OUTPUT);
    pinMode(RCLK1, OUTPUT);
    pinMode(RCLK2, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    for (int i = 0; i < 4; i++) {
        pinMode(bcdpin[i], OUTPUT);
    }
    Serial.begin(9600);

    //rotary encoder end
}

void loop() {
    for (int i = 0; i < 4; i++) {
        while (ButtonPressed() == 0) {
            int digit = RotaryEncoder();
            if (i == 0) {
                //BCD
                decoder(digit);
                code[0] = digit;
            }
            if (i == 1) {
                //Shift Register 1
                shiftOutData1(segmentData[digit]);
                code[1] = digit;
            }
            if (i == 2) {
                //Shift Register 2
                shiftOutData2(segmentData[digit]);
                code[2] = digit;
            }
            if(i==3);
            //keeps the code static on the displays. as soon as you press again, u go through.
        }
    }
    for(int i=0; i<3; i++) Serial.println(code[i]);
}