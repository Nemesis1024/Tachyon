#include <Joystick.h>

#define JOYSTICK_COUNT 4

//analog input/output not implemented yet

//Joystick_ Joystick[JOYSTICK_COUNT] = {
//  Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD, 10, 1, false, false, false, false, false, false, false, false, false, false, false),
//  Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD, 10, 1, false, false, false, false, false, false, false, false, false, false, false),
//  Joystick_(0x05, JOYSTICK_TYPE_GAMEPAD, 10, 1, false, false, false, false, false, false, false, false, false, false, false),
//  Joystick_(0x06, JOYSTICK_TYPE_GAMEPAD, 10, 1, false, false, false, false, false, false, false, false, false, false, false)
//};

boolean debug = false;

unsigned long startingMicros;
unsigned long currentMicros;

boolean USBOutput = false;

byte clockStatus;
byte p1Clocks;
byte p2Clocks;

int controllerInput;
boolean controllerData[16];
boolean prevControllerData[4][16];
boolean snesControllerData[4][16];

void setup() {
  if (debug == true) {
    Serial.begin(9600);
  }
  Serial1.begin(115200); //Try making this SUPER high 32u4 should be able to go up to 2Mbps!
  //  if (USBOutput == true) {
  //    for (int i = 0; i < JOYSTICK_COUNT; i++) {
  //      Joystick[i].begin(false);
  //    }
  //  }
  //Setup SNES controller port interface pins and interrupt
  attachInterrupt(1, snesLatching, RISING); //SNES player 1 latch signal is cnnected to INT1 (pin 2)
  pinMode(3, INPUT); //SNES player 1 clock signal is connected to pin 3
  pinMode(4, OUTPUT); //SNES player 1 data line is connected to pin 4
  pinMode(6, INPUT); //SNES player 2 clock signal is connected to pin 6
  pinMode(5, OUTPUT); //SNES player 2 data line is connected to pin 5
}


void shakeHands() { // ******add FASTRUN to all functions except setup!!******
  int greeting = 0;
  //  delay(2000);
  while (greeting != 216) {
    if (Serial1.available()) {
      greeting = Serial1.read();
      if (debug == true) {
        Serial.print("Greeting recieved: ");
        Serial.println(greeting);
      }
      if (greeting == 255) {
        Serial1.write(4);
        if (debug == true) {
          Serial.println("Resetting");
        }
      }
      if (greeting == 253) {
        Serial1.write(1);
      }
    }
  }
  Serial1.write(2);
  if (debug == true) {
    Serial.println("Contact established!");
  }
}


void readControllerInput() {
  startingMicros = micros();
  int currentController;
  if (Serial1.available() >= 2) {
    controllerInput = Serial1.read();
    if (debug == true) {
      Serial.print("Input recieved: ");
      Serial.println(controllerInput);
    }
    if (controllerInput == 255) {
      Serial1.write(4);
      if (debug == true) {
        Serial.println("Resetting");
      }
      shakeHands();
      return;
    }
    for (int i = 7; i >= 0; i--) {
      controllerData[i] = bitRead(controllerInput, i);
    }
    controllerInput = Serial1.read();
    if (debug == true) {
      Serial.print("Input recieved: ");
      Serial.println(controllerInput);
    }
    if (controllerInput == 255) {
      Serial1.write(4);
      shakeHands();
      return;
    }
    for (int i = 7; i >= 0; i--) {
      controllerData[i + 8] = bitRead(controllerInput, i);
    }
    if (debug == true) {
      for (int i = 0; i <= 15; i++) {
      Serial.print(controllerData[i]);
      }
      Serial.println();
    }
    if (controllerData[14] == 1
        && controllerData[15] == 1) { //controller data is for player 1
      currentController = 0;
    }
    if (controllerData[14] == 0
        && controllerData[15] == 1) { //controller data is for player 2
      currentController = 1;
    }
    if (controllerData[14] == 1
        && controllerData[15] == 0) { //controller data is for player 3
      currentController = 2;
    }
    if (controllerData[14] == 0
        && controllerData[15] == 0) { //controller data is for player 4
      currentController = 3;
    }
    mapSNESOutputBits(currentController);
    //    if (USBOutput == true) {
    //      sendUSBControllerOutput(currentController);
    //    }
  }
}

void mapSNESOutputBits(int playerNum) {
  if (controllerData[0]) snesControllerData[playerNum][0] = 1; //SNES B
  else snesControllerData[playerNum][0] = 0;
  if (controllerData[2]) snesControllerData[playerNum][1] = 1; //SNES Y
  else snesControllerData[playerNum][1] = 0;
  if (controllerData[6]) snesControllerData[playerNum][2] = 1; //SNES Select
  else snesControllerData[playerNum][2] = 0;
  if (controllerData[7]) snesControllerData[playerNum][3] = 1; //SNES Start
  else snesControllerData[playerNum][3] = 0;
  if (controllerData[12]) snesControllerData[playerNum][4] = 1; //SNES Up
  else snesControllerData[playerNum][4] = 0;
  if (controllerData[13]) snesControllerData[playerNum][5] = 1; //SNES Down
  else snesControllerData[playerNum][5] = 0;
  if (controllerData[10]) snesControllerData[playerNum][6] = 1; //SNES Left
  else snesControllerData[playerNum][6] = 0;
  if (controllerData[11]) snesControllerData[playerNum][7] = 1; //SNES Right
  else snesControllerData[playerNum][7] = 0;
  if (controllerData[1]) snesControllerData[playerNum][8] = 1; //SNES A
  else snesControllerData[playerNum][8] = 0;
  if (controllerData[3]) snesControllerData[playerNum][9] = 1; //SNES X
  else snesControllerData[playerNum][9] = 0;
  if (controllerData[4]) snesControllerData[playerNum][10] = 1; //SNES L
  else snesControllerData[playerNum][10] = 0;
  if (controllerData[5]) snesControllerData[playerNum][11] = 1; //SNES R
  else snesControllerData[playerNum][11] = 0;
  Serial1.write(1); // Send ack/ready to Processing
  if (debug == true) {
    currentMicros = micros();
    Serial.print("Time to process SNES output (in us):");
    Serial.println(currentMicros - startingMicros);
  }
}

//void sendUSBControllerOutput(int playerNum) {
//  for (int i = 0; i <= 9; i++) {
//    if (controllerData[i] != prevControllerData[playerNum][i]) {
//      Joystick[playerNum].setButton(i, controllerData[i]);
//      prevControllerData[playerNum][i] = controllerData[i];
//    }
//  }
//  if (controllerData[10] != prevControllerData[playerNum][10]
//      || controllerData[11] != prevControllerData[playerNum][11]
//      || controllerData[12] != prevControllerData[playerNum][12]
//      || controllerData[13] != prevControllerData[playerNum][13]) {
//    if ((controllerData[10] == 0)
//        && (controllerData[11] == 0)
//        && (controllerData[12] == 0)
//        && (controllerData[13] == 0)) { //If dPad is not pressed
//      Joystick[playerNum].setHatSwitch(0, -1);
//    }
//    if ((controllerData[12] == 1)
//        && (controllerData[11] == 0)
//        && (controllerData[10] == 0)) { //If up is pressed
//      Joystick[playerNum].setHatSwitch(0, 0);
//    }
//    if ((controllerData[12] == 1)
//        && (controllerData[11] == 1)) { //If up-right is pressed
//      Joystick[playerNum].setHatSwitch(0, 45);
//    }
//    if ((controllerData[11] == 1)
//        && (controllerData[12] == 0)
//        && (controllerData[13] == 0)) { //If right is pressed
//      Joystick[playerNum].setHatSwitch(0, 90);
//    }
//    if ((controllerData[11] == 1)
//        && (controllerData[13] == 1)) { //If down-right is pressed
//      Joystick[playerNum].setHatSwitch(0, 135);
//    }
//    if ((controllerData[13] == 1)
//        && (controllerData[11] == 0)
//        && (controllerData[10] == 0)) { //If down is pressed
//      Joystick[playerNum].setHatSwitch(0, 180);
//    }
//    if ((controllerData[13] == 1)
//        && (controllerData[10] == 1)) { //If down-left is pressed
//      Joystick[playerNum].setHatSwitch(0, 225);
//    }
//    if ((controllerData[10] == 1)
//        && (controllerData[13] == 0)
//        && (controllerData[12] == 0)) { //If left is pressed
//      Joystick[playerNum].setHatSwitch(0, 270);
//    }
//    if ((controllerData[10] == 1)
//        && (controllerData[12] == 1)) { //If up-left is pressed
//      Joystick[playerNum].setHatSwitch(0, 315);
//    }
//    for (int i = 10; i <= 13; i++) {
//      prevControllerData[playerNum][i] = controllerData[i];
//    }
//  }
//  if (USBOutput == true) {
//    Joystick[playerNum].sendState();
//  }
//  Serial1.write(1); // Send ack/ready to Processing
//  if (debug == true) {
//    currentMicros = micros();
//    Serial.print("Time to process USB output (in us):");
//    Serial.println(currentMicros - startingMicros);
//  }
//}

void snesLatching() {
  // Set data lines for first button
  if (snesControllerData[0][0]) PORTD &= B11101111;
  else PORTD |= B00010000;
  if (snesControllerData[1][0]) PORTC &= B10111111;
  else PORTC |= B01000000;

  // Clocking loop
  p1Clocks = 1; p2Clocks = 1;
  boolean p1ClockStatus = HIGH; boolean p2ClockStatus = HIGH;
  boolean snesClocking = true;
  int loopTimer = 0;
  while (snesClocking == true) {
    clockStatus = PIND; // Read PIND to a variable to minimize port reads for speed
    loopTimer++; // Increment loop timer to allow for ISR timeout

    // If P1 clock goes low
    if (!(clockStatus & B00000001)) {
      p1ClockStatus = LOW;
    }

    // If P1 was low then went high
    if ((p1ClockStatus == LOW) && (clockStatus & B00000001)) {
      p1ClockStatus = HIGH;
      if (snesControllerData[0][p1Clocks]) PORTD &= B11101111;
      else PORTD |= B00010000;
      p1Clocks++;
    }

    // If P2 clock goes low
    if (!(clockStatus & B10000000)) {
      p2ClockStatus = LOW;
    }

    // If P2 was low then went high
    if ((p2ClockStatus == LOW) && (clockStatus & B10000000)) {
      p2ClockStatus = HIGH;
      if (snesControllerData[1][p2Clocks]) PORTC &= B10111111;
      else PORTC |= B01000000;
      p2Clocks++;
    }

    // Check if clocking is done
    if (p1Clocks >= 17 || p2Clocks >= 17) { // If either P1 or P2 is finished clocking
      if ((p1Clocks + p2Clocks) == 18 || (p1Clocks + p2Clocks) >= 30) { // Only one has been clocking or both are done clocking
        PORTD &= B11101111; // Set P1 data line low
        PORTC &= B10111111; // Set P2 data line low
        snesClocking = false; // Exit clocking loop
      }
    }
    // Check for timeout event and send debug data
    if (loopTimer > 200) { // If the ISR has been running for longer than ~380 microseconds (it should only take 204)
      if (debug == true) {
        Serial.print ("p1Clocks: ");
        Serial.println (p1Clocks);
        Serial.print ("p1ClockStatus: ");
        Serial.println (p1ClockStatus);
        Serial.print ("p2Clocks: ");
        Serial.println (p2Clocks);
        Serial.print ("p2ClockStatus: ");
        Serial.println (p2ClockStatus);
      }
      snesClocking = false; // Exit clocking loop
    }
  }
  //Serial1.write(3);
}



void loop() {
  readControllerInput();
}
