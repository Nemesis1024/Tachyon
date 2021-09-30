boolean debug = true;

unsigned long startingMicros;
unsigned long currentMicros;

word consoleOutput;

word controllerInput;
boolean prevControllerData[4][16];
word controllerOutput[4];

word gen3P1Map[2];
word gen6P1Map[2][4];
word gen3P2Map[2];
word gen6P2Map[2][4];

int gen6P1Cycle = 0;
unsigned long gen6P1CycleTime;
int gen6P2Cycle = 0;
unsigned long gen6P2CycleTime;

HardwareSerial Serial2(PA3, PA2);

void setup() {
  if (debug == true) {
    delay (1000);
    Serial.begin(9600);
    Serial.println("Debug mode enabled");
  }
  Serial2.begin(115200);
}


void shakeHands() {
  word greeting = 0;
  byte firstbyte = 0;
  byte secondbyte = 0;
  while (greeting != 18537) { //"Hi"
    if (Serial2.available() >= 2) {
      firstbyte = Serial2.read();
      secondbyte = Serial2.read();
      greeting = (secondbyte << 8) | (firstbyte);
      if (debug == true) {
        Serial.print("Greeting recieved: ");
        Serial.println(greeting);
      }
      if (greeting == 65535) {
        Serial2.write(4);
        if (debug == true) {
          Serial.println("Resetting");
        }
      }
      else if (greeting == 20037) { //"NE"
        Serial2.write(1);
        consoleOutput = greeting;
        snesSetup();
        if (debug == true) {
          Serial.println("Outputting S/NES");
        }
      }
      else if (greeting == 18227) { //"G3"
        Serial2.write(1);
        consoleOutput = greeting;
        genSetup();
        if (debug == true) {
          Serial.println("Outputting Genesis 3 button");
        }
      }
      else if (greeting == 18230) { //"G6"
        Serial2.write(1);
        consoleOutput = greeting;
        genSetup();
        if (debug == true) {
          Serial.println("Outputting Genesis 6 button");
        }
      }
      else if (greeting != 18537) {
        Serial2.write(0);
        if (debug == true) {
          Serial.print("Unexpected response from host: ");
          Serial.println(greeting);
        }
      }
    }
  }
  Serial2.write(2);
  if (debug == true) {
    Serial.println("Contact established!");
  }
}


void readControllerInput() {
  if (Serial2.available() >= 2) {
    startingMicros = micros();
    int currentController;
    byte firstbyte = Serial2.read();
    byte secondbyte = Serial2.read();
    controllerInput = (firstbyte) | (secondbyte << 8);
    if (debug == true) {
      Serial.print("Input recieved: ");
      Serial.println(controllerInput);
    }
    if (controllerInput == 65535 || !consoleOutput) {
      Serial2.write(4);
      if (debug == true) {
        Serial.println("Resetting");
      }
      shakeHands();
      return;
    }
    currentController = (controllerInput >> 14); //the 2 MSB indicate controller number
    controllerInput = (controllerInput & 16383); //clear controller number from controllerData
    controllerOutput[currentController] = controllerInput;
    if (consoleOutput == 18227) {
      gen3Mapping();
    }
    if (consoleOutput == 18230) {
      gen6Mapping();
    }
    Serial2.write(1); // Send ack/ready to Processing
    if (debug == true) {
      currentMicros = micros();
      Serial.print("Time to process output (in us):");
      Serial.println(currentMicros - startingMicros);
    }
  }
}

void loop() {
  readControllerInput();
  if (consoleOutput == 18230) { //If output is Genesis 6 button
    gen6Timer();
  }
}