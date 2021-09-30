void snesSetup() { //Setup SNES controller port interface pins and interrupt
  pinMode(PA1, INPUT_PULLDOWN); //Need to pull down the latch to prevent latching from interference 
  attachInterrupt(digitalPinToInterrupt(PA1), snesLatching, RISING); //SNES player 1 latch signal is cnnected to PA1
  pinMode(PB4, INPUT_PULLDOWN); //Need to pull down the latch to prevent latching from interference 
  attachInterrupt(digitalPinToInterrupt(PB4), snesLatching, RISING); //SNES player 2 latch signal is cnnected to PB4
  pinMode(PA5, INPUT); //SNES player 1 clock signal is connected to PA5
  pinMode(PA6, OUTPUT); //SNES player 1 data line is connected to PA6
  pinMode(PB7, INPUT); //SNES player 2 clock signal is connected to PB7
  pinMode(PB0, OUTPUT); //SNES player 2 data line is connected to PB0
}


void snesLatching() {
  //Set data lines for first button
  bitWrite(GPIOA->ODR, 6, !bitRead(controllerOutput[0], 0)); //Set PA6 to the inverse value of the first button for player 1
  bitWrite(GPIOB->ODR, 0, !bitRead(controllerOutput[1], 0)); //Set PB0 to the inverse value of the first button for player 2

  //SNES clocking loop
  byte p1Clocks = 1;
  byte p2Clocks = 1;
  boolean p1ClockStatus;
  boolean p2ClockStatus;
  boolean prevP1ClockStatus = 1;
  boolean prevP2ClockStatus = 1;
  boolean snesClocking = true;
  while (snesClocking == true) {
    p1ClockStatus = bitRead(GPIOA->IDR, 5); //Set p1Clockstatus to the value of PA5
    p2ClockStatus = bitRead(GPIOB->IDR, 7); //Set p2Clockstatus to the value of PB7

    //If P1 was high then went low
    if ((prevP1ClockStatus == 1) && (p1ClockStatus == 0)) {
      prevP1ClockStatus = 0;
    }

    //If P1 was low then went high
    if ((prevP1ClockStatus == 0) && (p1ClockStatus == 1)) {
      prevP1ClockStatus = 1;
      bitWrite(GPIOA->ODR, 6, !bitRead(controllerOutput[0], p1Clocks)); //Set PA6 to the inverse value of the currently clocking button for player 1
      p1Clocks++;
    }

    //If P2 was high then went low
    if ((prevP2ClockStatus == 1) && (p2ClockStatus == 0)) {
      prevP2ClockStatus = 0;
    }

    //If P2 was low then went high
    if ((prevP2ClockStatus == 0) && (p2ClockStatus == 1)) {
      prevP2ClockStatus = 1;
      bitWrite(GPIOB->ODR, 0, !bitRead(controllerOutput[1], p2Clocks)); //Set PB0 to the inverse value of the currently clocking button for player 2
      p2Clocks++;
    }

    //Check if clocking is done
    if (p1Clocks >= 17 || p2Clocks >= 17) { //If either P1 or P2 is finished clocking
      if ((p1Clocks + p2Clocks) == 18 || (p1Clocks + p2Clocks) >= 30) { //Only one has been clocking or both are done clocking
        bitWrite(GPIOA->ODR, 6, 0);  //Set P1 data line low
        bitWrite(GPIOB->ODR, 0, 0); //Set P2 data line low
        snesClocking = false; //Exit clocking loop
      }
    }
  }
  EXTI->PR |= 18; // Clear pending interrupts on lines 1 & 4 to prevent double latch interrupts when both controllers connected
}