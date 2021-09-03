#include <Joystick.h>

#define JOYSTICK_COUNT 4

//analog input/output not implemented yet

//Joystick_ Joystick[JOYSTICK_COUNT] = {
//  Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD, 10, 1, false, false, false, false, false, false, false, false, false, false, false),
//  Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD, 10, 1, false, false, false, false, false, false, false, false, false, false, false),
//  Joystick_(0x05, JOYSTICK_TYPE_GAMEPAD, 10, 1, false, false, false, false, false, false, false, false, false, false, false),
//  Joystick_(0x06, JOYSTICK_TYPE_GAMEPAD, 10, 1, false, false, false, false, false, false, false, false, false, false, false)
//};

boolean debug = true;

unsigned long startingMicros;
unsigned long currentMicros;

boolean USBOutput = false;
boolean SNES = false;

byte clockStatus;
byte p1Clocks;
byte p2Clocks;

word controllerInput;
boolean prevControllerData[4][16];
word snesControllerData[4];

void setup() {
  if (debug == true) {
    delay (1000);
    Serial.begin(9600);
    Serial.println("Debug mode enabled");
  }
  Serial1.begin(115200); //Try making this SUPER high 32u4 should be able to go up to 2Mbps!
  //  if (USBOutput == true) {
  //    for (int i = 0; i < JOYSTICK_COUNT; i++) {
  //      Joystick[i].begin(false);
  //    }
  //  }
  //Setup SNES controller port interface pins and interrupt
  attachInterrupt(1, snesLatching, RISING); //SNES latch signal is cnnected to INT1 (pin 2)
  pinMode(3, INPUT); //SNES player 1 clock signal is connected to pin 3
  pinMode(4, OUTPUT); //SNES player 1 data line is connected to pin 4
  pinMode(6, INPUT); //SNES player 2 clock signal is connected to pin 6
  pinMode(5, OUTPUT); //SNES player 2 data line is connected to pin 5
}


void shakeHands() {
  word greeting = 0;
  byte firstbyte = 0;
  byte secondbyte = 0;
  while (greeting != 18537) { //"Hi"
    if (Serial1.available() >= 2) {
      firstbyte = Serial1.read();
      secondbyte = Serial1.read();
      greeting = (secondbyte << 8) | (firstbyte);
      if (debug == true) {
        Serial.print("Greeting recieved: ");
        Serial.println(greeting);
      }
      if (greeting == 65535) {
        Serial1.write(4);
        if (debug == true) {
          Serial.println("Resetting");
        }
      }
      if (greeting == 20037) { //"NE"
        Serial1.write(1);
        SNES = true;
        if (debug == true) {
          Serial.println("Outputting S/NES");
        }
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
    byte firstbyte = Serial1.read();
    byte secondbyte = Serial1.read();
    controllerInput = (firstbyte) | (secondbyte << 8);
    if (debug == true) {
      Serial.print("Input recieved: ");
      Serial.println(controllerInput);
    }
    if (controllerInput == 65535) {
      Serial1.write(4);
      if (debug == true) {
        Serial.println("Resetting");
      }
      shakeHands();
      return;
    }
    currentController = (controllerInput >> 14); //the 2 MSB indicate controller number
    controllerInput = (controllerInput & 16383); //clear controller number from controllerData
    if (SNES == true) {
      snesControllerData[currentController] = controllerInput;
    }
    //    if (USBOutput == true) {
    //      sendUSBControllerOutput(currentController);
    //    }
    Serial1.write(1); // Send ack/ready to Processing
    if (debug == true) {
      currentMicros = micros();
      Serial.print("Time to process output (in us):");
      Serial.println(currentMicros - startingMicros);
    }
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
  if (bitRead(snesControllerData[0], 0)) PORTD &= B11101111;
  else PORTD |= B00010000;
  if (bitRead(snesControllerData[1], 0)) PORTC &= B10111111;
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
      if (bitRead(snesControllerData[0], p1Clocks)) PORTD &= B11101111;
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
      if (bitRead(snesControllerData[1], p2Clocks)) PORTC &= B10111111;
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
}



void loop() {
  readControllerInput();
}
