#include "header.h"

void setup() {
  myServo.attach(SERVO_PIN);
  myServo.write(CLOSED);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(reset_button, INPUT_PULLUP);

  itteration = EEPROM.read(4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextWrap(true);
  display.display();

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
}

void loop() {
  // put everything to 0
  int digit = 0;
  shiftOutData1(segmentData[digit]);
  shiftOutData2(segmentData[digit]);
  decoder(digit);

// start of the loop
// state of the displays
  for (int i = 0; i < 4; i++) {
    //if rotary button is pressed, change display
    while (ButtonPressed() == 0) {
      digit = RotaryEncoder();
      if (i == 0) {
        decoder(digit);
        code[0] = digit;
      }
      if (i == 1) {
        shiftOutData1(segmentData[digit]);
        code[1] = digit;
      }
      if (i == 2) {
        shiftOutData2(segmentData[digit]);
        code[2] = digit;
      }
      //last state is a check state.
      // if u press the reset button during this state -> redo your first code (only works on itteration 0)
      if(i==3) {
        if(digitalRead(reset_button) == LOW && itteration == 0){
          itteration = -1;
        }
      }
    }
  }
  //if first time going through the loop, store the new code in eeprom and close door
  if(itteration == 0){
    setNewPassword();
    door_closed();
  }
  //if u've been through the loop already check password with eeprom
  if(itteration >= 1){
    if(checkPassword() == true){ //correct? open the door and reset everything
      door_open();
      penalty = 0;
      itteration = -1;
    }
    else{ //wrong?, add a waiting time till u can continue. add on to the penalty time everytime u do this.
      incorrect_password();
      delay(5000 + penalty*5000);
      penalty++;
    }
  }
  itteration++;
  EEPROM.update(4,itteration); // keep track of the itteration value
}