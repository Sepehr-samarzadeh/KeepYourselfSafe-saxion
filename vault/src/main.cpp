#include <Arduino.h>
#include <EEPROM.h>
#include <stdbool.h>
#include <Servo.h>
#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//SERVO
#define SERVO_PIN 13
#define SERVO_POS 90
#define OPEN 90
#define CLOSED 0
#define BUZZERPIN A1 //change this
#define LED_GREEN A0// change this
#define LED_RED A2
Servo myServo;

///OLED Display
#define WIDTH 128
#define HEIGHT 64
Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1);

//EEprom
#define reset_button A3
//rotary encoder
#define outputA 12
#define outputB 11
#define button 9
//shiftregister
#define RCLK1 10
#define SER 6
#define RCLK2 7
#define SRCLK 8

int penalty = 0;
int itteration = 0;
const int passwordLength = 3;  // Change the password length as needed
int storedPassword[passwordLength] = {0,0,0}; // put in the passsword
int code[passwordLength];
int segmentData[10] = { 64, 121, 36, 48, 25, 18, 2, 120, 0, 24 };
//int pressed;
const int bcdpin[4] = { 2, 3, 4, 5 };

//Anita's door and OLED
void incorrect_password(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0,30);
    display.println("Incorrect password");
    display.display();
    playBipSound();
}

void vault_closed(){
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(0,30);
    display.println("Closed");
    display.display();
}

void playBipBipSound() {
    tone(BUZZERPIN, 1000);  // Play a tone at 1000 Hz
    delay(200);            // Delay for 200 milliseconds
    noTone(BUZZERPIN);      // Stop the tone
    delay(200);            // Pause for 200 milliseconds
    tone(BUZZERPIN, 1000);  // Play another tone at 1000 Hz
    delay(200);            // Delay for 200 milliseconds
    noTone(BUZZERPIN);      // Stop the tone
}

void playBipSound() {
    tone(BUZZERPIN, 1000);  // Play a tone at 1000 Hz for 200 milliseconds
    delay(2000);
    noTone(BUZZERPIN);      // Stop the tone
}

void door_open(){

    myServo.write(OPEN);
    for(int pos = 0; pos <SERVO_POS; ++pos){
        delay(10);
    }
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
    vault_open();
    playBipBipSound();

}

void door_closed(){
    myServo.write(CLOSED);
    for(int pos = 90; pos >= 0; --pos){
        delay(10);
    }
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
    vault_closed();
    playBipBipSound();
}


void vault_open(){
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(0,30);
    display.println("OPEN!");
    display.display();
}


//EEPROM
bool checkPassword() {
    int count = 0;
    for(int i = 0; i<3; i++){
        storedPassword[i] = EEPROM.read(i);
    }
    for(int i = 0; i < 3; i++){
        if(storedPassword[i] == code[i]){
            count++;
        }
    }
    if (count == 3){
        count= 0;
        return true;
    }
    else {
        count= 0;
        return false;
    }
}

void setNewPassword() {
    for(int i = 0; i<3; i++){
        EEPROM.update(i, code[i]);
    }
}

//ROTARY ENCODER
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

//SHIFT REGISTER 1
void shiftOutData1(int ShiftNum) {
    digitalWrite(RCLK1, LOW);
    shiftOut(SER, SRCLK, MSBFIRST, ShiftNum);
    digitalWrite(RCLK1, HIGH);
}
//Shift Register 2
void shiftOutData2(int ShiftNum) {
    digitalWrite(RCLK2, LOW);
    shiftOut(SER, SRCLK, MSBFIRST, ShiftNum);
    digitalWrite(RCLK2, HIGH);
}

//BCD
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
    //Anita's stuff
    // put your setup code here, to run once:
    myServo.attach(SERVO_PIN);
    myServo.write(CLOSED);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(BUZZERPIN, OUTPUT);
    pinMode(reset_button, INPUT_PULLUP);

    itteration = EEPROM.read(4);

// the second parameter is the address (either 0x3D or 0x3C)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextWrap(true); //This moves long text to a newline
    display.display();

    //rotary encoder
    pinMode(outputA, INPUT);
    pinMode(outputB, INPUT);
    pinMode(button, INPUT_PULLUP);
    //Shift Registers
    pinMode(SER, OUTPUT);
    pinMode(RCLK1, OUTPUT);
    pinMode(RCLK2, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    //BCD
    for (int i = 0; i < 4; i++) {
        pinMode(bcdpin[i], OUTPUT);
    }
    Serial.begin(9600);
}

void loop() {
    // put everything to 0.
    int digit = 0;
    shiftOutData1(segmentData[digit]);
    shiftOutData2(segmentData[digit]);
    decoder(digit);
    //start asking for input for code
    for (int i = 0; i < 4; i++) {
        //everytime a digit is entered, switch the display
        while (ButtonPressed() == 0) {
            // allow the user to determine their code
            digit = RotaryEncoder();
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
            if(i==3) {
                //Static state to see your code.
                if(digitalRead(reset_button) == LOW && itteration == 0){
                    itteration = -1;
                }
            }
        }
    }
    if(itteration == 0){
        setNewPassword();
        door_closed();
    }
    if(itteration >= 1){
        if(checkPassword() == true){
            door_open();
            penalty = 0;
            itteration = -1;
        }
        else{
            incorrect_password();
            delay(5000 + penalty*5000);
            penalty++;
            //make a pib sound
        }
    }
    itteration++;
    EEPROM.update(4, itteration);
}


