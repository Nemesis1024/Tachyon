void genSetup () { //Setup Genesis controller port interface pins and interrupt
  if (consoleOutput == 18227) {
    attachInterrupt(digitalPinToInterrupt(PA0), gen3P1Switching, CHANGE); //Gensis player 1 switch signal (TH) is cnnected to PA0
    attachInterrupt(digitalPinToInterrupt(PB4), gen3P2Switching, CHANGE); //Gensis player 2 switch signal (TH) is cnnected to PB4
  }
  if (consoleOutput == 18230) {
    attachInterrupt(digitalPinToInterrupt(PA0), gen6P1Switching, CHANGE); //Gensis player 1 switch signal (TH) is cnnected to PA0
    attachInterrupt(digitalPinToInterrupt(PB4), gen6P2Switching, CHANGE); //Gensis player 2 switch signal (TH) is cnnected to PB4
  }

  pinMode(PA5, OUTPUT); //Gensis player 1 U signal is connected to pin A5
  pinMode(PA6, OUTPUT); //Gensis player 1 D signal is connected to pin A6
  pinMode(PA7, OUTPUT); //Gensis player 1 L signal is connected to pin A7
  pinMode(PA8, OUTPUT); //Gensis player 1 R signal is connected to pin A8
  pinMode(PA9, OUTPUT); //Gensis player 1 B&A (TL) signal is connected to pin A9
  pinMode(PA10, OUTPUT); //Gensis player 1 C&Start (TR) signal is connected to pin A10

  pinMode(PB7, OUTPUT); //Gensis player 2 U signal is connected to pin B7
  pinMode(PB0, OUTPUT); //Gensis player 2 D signal is connected to pin B0
  pinMode(PB1, OUTPUT); //Gensis player 2 L signal is connected to pin B1
  pinMode(PB2, OUTPUT); //Gensis player 2 R signal is connected to pin B2
  pinMode(PB3, OUTPUT); //Gensis player 2 B&A (TL) signal is connected to pin B3
  pinMode(PB6, OUTPUT); //Gensis player 2 C&Start (TR) signal is connected to pin B6
}

void gen3Mapping () {

  bitWrite(gen3P1Map[1], 5, !bitRead(controllerOutput[0], 4)); //P1 Up
  bitWrite(gen3P1Map[0], 5, !bitRead(controllerOutput[0], 4));
  bitWrite(gen3P1Map[1], 6, !bitRead(controllerOutput[0], 5)); //P1 Down
  bitWrite(gen3P1Map[0], 6, !bitRead(controllerOutput[0], 5));
  bitWrite(gen3P1Map[1], 7, !bitRead(controllerOutput[0], 6)); //P1 Left
  bitWrite(gen3P1Map[0], 7, 0);
  bitWrite(gen3P1Map[1], 8, !bitRead(controllerOutput[0], 7)); //P1 Right
  bitWrite(gen3P1Map[0], 8, 0);
  bitWrite(gen3P1Map[1], 9, !bitRead(controllerOutput[0], 0)); //P1 B (TL)
  bitWrite(gen3P1Map[0], 9, !bitRead(controllerOutput[0], 1)); //P1 A (TL)
  bitWrite(gen3P1Map[1], 10, !bitRead(controllerOutput[0], 8)); //P1 C (TR)
  bitWrite(gen3P1Map[0], 10, !bitRead(controllerOutput[0], 3)); //P1 Start (TR)

  bitWrite(gen3P2Map[1], 7, !bitRead(controllerOutput[1], 4)); //P2 Up
  bitWrite(gen3P2Map[0], 7, !bitRead(controllerOutput[1], 4));
  bitWrite(gen3P2Map[1], 0, !bitRead(controllerOutput[1], 5)); //P2 Down
  bitWrite(gen3P2Map[0], 0, !bitRead(controllerOutput[1], 5));
  bitWrite(gen3P2Map[1], 1, !bitRead(controllerOutput[1], 6)); //P2 Left
  bitWrite(gen3P2Map[0], 1, 0);
  bitWrite(gen3P2Map[1], 2, !bitRead(controllerOutput[1], 7)); //P2 Right
  bitWrite(gen3P2Map[0], 2, 0);
  bitWrite(gen3P2Map[1], 3, !bitRead(controllerOutput[1], 0)); //P2 B (TL)
  bitWrite(gen3P2Map[0], 3, !bitRead(controllerOutput[1], 1)); //P2 A (TL)
  bitWrite(gen3P2Map[1], 6, !bitRead(controllerOutput[1], 8)); //P2 C (TR)
  bitWrite(gen3P2Map[0], 6, !bitRead(controllerOutput[1], 3)); //P2 Start (TR)
}

void gen6Mapping () {
  //Map P1 controller input for 1st polling cycle
  bitWrite(gen6P1Map[1][0], 5, !bitRead(controllerOutput[0], 4)); //P1 Up
  bitWrite(gen6P1Map[0][0], 5, !bitRead(controllerOutput[0], 4));
  bitWrite(gen6P1Map[1][0], 6, !bitRead(controllerOutput[0], 5)); //P1 Down
  bitWrite(gen6P1Map[0][0], 6, !bitRead(controllerOutput[0], 5));
  bitWrite(gen6P1Map[1][0], 7, !bitRead(controllerOutput[0], 6)); //P1 Left
  bitWrite(gen6P1Map[0][0], 7, 0);
  bitWrite(gen6P1Map[1][0], 8, !bitRead(controllerOutput[0], 7)); //P1 Right
  bitWrite(gen6P1Map[0][0], 8, 0);
  bitWrite(gen6P1Map[1][0], 9, !bitRead(controllerOutput[0], 8)); //P1 B (TL)
  bitWrite(gen6P1Map[0][0], 9, !bitRead(controllerOutput[0], 0)); //P1 A (TL)
  bitWrite(gen6P1Map[1][0], 10, !bitRead(controllerOutput[0], 11)); //P1 C (TR)
  bitWrite(gen6P1Map[0][0], 10, !bitRead(controllerOutput[0], 3)); //P1 Start (TR)

  //Map P1 controller input for 3rd high polling cycle
  bitWrite(gen6P1Map[1][3], 5, !bitRead(controllerOutput[0], 10)); //P1 Z
  bitWrite(gen6P1Map[1][3], 6, !bitRead(controllerOutput[0], 9)); //P1 Y
  bitWrite(gen6P1Map[1][3], 7, !bitRead(controllerOutput[0], 1)); //P1 X
  bitWrite(gen6P1Map[1][3], 8, !bitRead(controllerOutput[0], 2)); //P1 Mode
  bitWrite(gen6P1Map[1][3], 9, 1); //P1 TL
  bitWrite(gen6P1Map[1][3], 10, 1); //P1 TR

  //Map P1 controller input for additional polling cycles
  gen6P1Map[1][1] = gen6P1Map[1][0]; //Copy 1st cycle high to 2nd cycle high
  gen6P1Map[0][1] = gen6P1Map[0][0]; //Copy 1st cycle low to 2nd cycle low
  gen6P1Map[1][2] = gen6P1Map[1][0]; //Copy 1st cycle high to 3rd cycle high
  gen6P1Map[0][2] = gen6P1Map[0][0] & 65055; //6 button controller detection (U, D, L & R pressed)
  gen6P1Map[0][3] = gen6P1Map[0][0] | 480; //6 button controller final cycle

  //Map P2 controller input for 1st polling cycle
  bitWrite(gen6P2Map[1][0], 7, !bitRead(controllerOutput[1], 4)); //P2 Up
  bitWrite(gen6P2Map[0][0], 7, !bitRead(controllerOutput[1], 4));
  bitWrite(gen6P2Map[1][0], 0, !bitRead(controllerOutput[1], 5)); //P2 Down
  bitWrite(gen6P2Map[0][0], 0, !bitRead(controllerOutput[1], 5));
  bitWrite(gen6P2Map[1][0], 1, !bitRead(controllerOutput[1], 6)); //P2 Left
  bitWrite(gen6P2Map[0][0], 1, 0);
  bitWrite(gen6P2Map[1][0], 2, !bitRead(controllerOutput[1], 7)); //P2 Right
  bitWrite(gen6P2Map[0][0], 2, 0);
  bitWrite(gen6P2Map[1][0], 3, !bitRead(controllerOutput[1], 8)); //P2 B (TL)
  bitWrite(gen6P2Map[0][0], 3, !bitRead(controllerOutput[1], 0)); //P2 A (TL)
  bitWrite(gen6P2Map[1][0], 6, !bitRead(controllerOutput[1], 11)); //P2 C (TR)
  bitWrite(gen6P2Map[0][0], 6, !bitRead(controllerOutput[1], 3)); //P2 Start (TR)

  //Map P2 controller input for 3rd high polling cycle
  bitWrite(gen6P2Map[1][3], 7, !bitRead(controllerOutput[1], 10)); //P2 Z
  bitWrite(gen6P2Map[1][3], 0, !bitRead(controllerOutput[1], 9)); //P2 Y
  bitWrite(gen6P2Map[1][3], 1, !bitRead(controllerOutput[1], 1)); //P2 X
  bitWrite(gen6P2Map[1][3], 2, !bitRead(controllerOutput[1], 2)); //P2 Mode
  bitWrite(gen6P2Map[1][3], 3, 1); //P2 TL
  bitWrite(gen6P2Map[1][3], 6, 1); //P2 TR

  //Map P2 controller input for additional polling cycles
  gen6P2Map[1][1] = gen6P2Map[1][0]; //Copy 1st cycle high to 2nd cycle high
  gen6P2Map[0][1] = gen6P2Map[0][0]; //Copy 1st cycle low to 2nd cycle low
  gen6P2Map[1][2] = gen6P2Map[1][0]; //Copy 1st cycle high to 3rd cycle high
  gen6P2Map[0][2] = gen6P2Map[0][0] & 65400; //6 button controller detection (U, D, L & R pressed)
  gen6P2Map[0][3] = gen6P2Map[0][0] | 135; //6 button controller final cycle

}

void gen3P1Switching() {
  GPIOA->ODR = gen3P1Map[bitRead(GPIOA->IDR, 0)];
}

void gen3P2Switching() {
  GPIOB->ODR = gen3P2Map[bitRead(GPIOB->IDR, 4)];
}

void gen6P1Switching() {
  GPIOA->ODR = gen6P1Map[bitRead(GPIOA->IDR, 0)][gen6P1Cycle];
  if (!bitRead(GPIOA->IDR, 0)) {
    gen6P1Cycle = ++gen6P1Cycle % 4; //Increment cycle count but roll back to 0 when the count reaches 4
  }
}

void gen6P2Switching() {
  GPIOB->ODR = gen6P2Map[bitRead(GPIOB->IDR, 4)][gen6P2Cycle];
  if (!bitRead(GPIOB->IDR, 4)) {
    gen6P2Cycle = ++gen6P2Cycle % 4; //Increment cycle count but roll back to 0 when the count reaches 4
  }
}

void gen6Timer() {
  if (gen6P1Cycle == 0) {
    gen6P1CycleTime = micros();
  }
  else if ((micros() - gen6P1CycleTime) > 1000) {
    gen6P1Cycle = 0;
  }
  if (gen6P2Cycle == 0) {
    gen6P2CycleTime = micros();
  }
  else if ((micros() - gen6P2CycleTime) > 1000) {
    gen6P2Cycle = 0;
  }
}