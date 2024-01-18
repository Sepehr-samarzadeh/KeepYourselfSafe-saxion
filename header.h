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
const int passwordLength = 3;  // it stores the password length
int storedPassword[passwordLength] = {0,0,0}; // put in the passsword
int code[passwordLength];
int segmentData[10] = { 64, 121, 36, 48, 25, 18, 2, 120, 0, 24 };
//int pressed;
const int bcdpin[4] = { 2, 3, 4, 5 };

//DESCRIPTION FOR THE BUZZER
/// this function makes the buzzer play a sound twice when called
void playBipBipSound();

/// this function makes the buzzer play a sound once when called
void playBipSound();

//SOURCE CODE FOR THE BUZZER

void playBipBipSound() {
  tone(BUZZERPIN, 1000);  // play a tone at 1000 Hz
  delay(200);            // delay for 200 milliseconds
  noTone(BUZZERPIN);      // stop the tone
  delay(200);            // pause for 200 milliseconds
  tone(BUZZERPIN, 1000);  // play another tone at 1000 Hz
  delay(200);            // delay for 200 milliseconds
  noTone(BUZZERPIN);      // stop the tone
}

void playBipSound() {
  tone(BUZZERPIN, 1000);  // play a tone at 1000 Hz for 200 milliseconds
  delay(2000);
  noTone(BUZZERPIN);      // stop the tone
}

//DESCRIPTION FOR THE EEPROM

/// this function compares the code stored with the input entered
/// calls the door_open function in the void loop function
bool checkPassword();

/// stores the input from the user in the eeprom
void setNewPassword();


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

//DESCRIPTION FOR THE OLED
/// this function prints incorrect password on the Oled display
/// calls when the user inputs the wrong password
void incorrect_password();

/// this function prints closed on the Oled display
/// calls when the user inputs the correct password
void vault_closed();

/// this function prints open on the Oled display
/// calls when the user compares the user's input
/// And it returns as the correct password
void vault_open();

//SOURCE CODE FOR THE OLED
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

void vault_open(){
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0,30);
  display.println("OPEN!");
  display.display();
}

//DESCRIPTION FOR THE SERVO
/// this function turns on the green_led
/// calls the buzzer function to play sound
/// call vault_open function which prints open on the oled
void door_open();

/// this function turns off the green_led
/// calls the buzzer function to play sound
/// calls the vault_close function which prints the vault is closed
void door_closed();

//SOURCE CODE FOR THE SERVO
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

//DESCRIPTION FOR THE ROTARY ENCODER
/** this function has three variables to compare the states
// if there has been a change increment counter
 * It reads three digits from the user
 * @return counter from the number 0-9
 // counter represnts a digit 
 */
int RotaryEncoder();

/** This function changes states when the button of the rotary encoder has been pressed
 * It compares a previous state from the current state and updates it when needed
 * @return true if the button has not been pressed
 */
int ButtonPressed();

//SOURCE CODE FOR THE ROTARY ENCODER
int RotaryEncoder() {
  static unsigned long prev_time_of_change_in_A = 0;
  int static old = LOW;  // old value of A
  int b;
  int a;
  static int counter = 0;
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

//DESCRIPTION FOR THE SHIFT REGISTERS
/**this function takes data from the rotary encoder and send it to the 7-segemnt display
 * the ShiftNum is the number sent to the 7-segment display
 * @param ShiftNum data from rotary encoder 
 * this is for the first shift register 
 */
void shiftOutData1(int ShiftNum);

/**this function takes data from the rotary encoder and send it to the 7-segemnt display
 * the ShiftNum is the number sent to the 7-segment display
 * @param ShiftNum data from rotary encoder 
 * this is for the second shift register 
 */ 
void shiftOutData2(int ShiftNum);

//SOURCE CODE FOR THE SHIFT REGISTERS

//SHIFT REGISTER 1
void shiftOutData1(int ShiftNum) {
  digitalWrite(RCLK1, LOW); // resting state of latch 
  shiftOut(SER, SRCLK, MSBFIRST, ShiftNum); //data,clock, most significatnt bit, the number shifted 
  digitalWrite(RCLK1, HIGH); //pulse the latch
}
//Shift Register 2
void shiftOutData2(int ShiftNum) {
  digitalWrite(RCLK2, LOW); // resting state of latch
  shiftOut(SER, SRCLK, MSBFIRST, ShiftNum); //data,clock, most significatnt bit, the number shifted 
  digitalWrite(RCLK2, HIGH); //pulse the latch
}

//DESCRIPTION FOR THE BCD DECODER
/**this function takes data from rotary encoder
 *turns rotary encoder data from decimal to binary
 * then send the data to 7-segment display
 * @param bcdNumber
 */
void decoder(int bcdNumber);

//SOURCE CODE FOR THE BCD DECODER
void decoder(int bcdNumber) {
  int bcdData[4] = { 0, 0, 0, 0 };
  for (int j = 0; j < 4; j++) {
    bcdData[j] = bcdNumber % 2;  //change the input data to the binary
    bcdNumber = bcdNumber / 2;  
  }
  for (int k = 0; k < 4; k++) {
    digitalWrite(bcdpin[k], bcdData[k]);
  }
}