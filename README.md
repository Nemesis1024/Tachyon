
# Tachyon

Use your web browser (currently Chrome version 78 or newer only) to capture game pad inputs and forward them to a microcontroller over Serial.
The Arduino sketch then sends the inputs to a classic video game console (currently NES, SNES & Genesis are supported) or USB via DInput (coming soon).
Support for up to 4 game pads in and out (coming soon).

The main goal for this project is to allow people to play games over [Parsec](https://parsec.app) on real consoles. 
Parsec passes external player controllers through to the host machine as virtual XInput (Xbox 360) controllers. 
This will capture the inputs from those controllers and forward them on to the target console, emulating a native controller.
You will obviously need a **low latency** capture card to capture the video from the console on the Parsec host machine. I use a Datapath Vision E1 for this. 

The web interface also provides a controller input display for all of the controllers with skins for each supported output console. 
This part was heavily inspired by [Gamepad Viewer](https://gamepadviewer.com) by mrmcpowned but has been written mostly from scratch.

Try it out here: [https://nemesis1024.github.io/Tachyon/](https://nemesis1024.github.io/Tachyon/)

You will need to enable Chrome's **Experimental Web Platform Features** by visiting [chrome://flags/#enable-experimental-web-platform-features](chrome://flags/#enable-experimental-web-platform-features)
_(copy the link and paste in a new tab)_

This is in **alpha (maybe beta now?)** with **some bugs**, typos, and ugly code.

Upcoming functionality:

- ~~Better serial error handling~~
- ~~More stable and efficient serial communication~~
- ~~NES skin~~
- ~~Mutiple (4) controller inputs/outputs~~
- ~~Make web interface prettier and skins scalable~~
- ~~Add customizable background color (useful for chroma key)~~
- ~~Genesis/Mega Drive output and skins (3 & 6 button)~~
- ~~Give the project an actual name~~
- PSX output and skin
- N64 output and skin
- USB output and skin
- Multitap emulation
- Controller to controller remapping (maybe)
- Button remapping (maybe)
- Additional console support
- A neat little hardware extra ;)

## Further information

The hardware used is a [generic STM32F401CCU6](https://www.amazon.com/gp/product/B07XBWGF9M) (the [WeAct Black Pill V1.2](https://stm32-base.org/boards/STM32F401CCU6-WeAct-Black-Pill-V1.2) is probably a better choice) and a [generic CP2104 USB to Serial Converter](https://www.amazon.com/NOYITO-CP2104-Serial-Converter-Adapter/dp/B07F5FPQH) 
but the one [from Adafruit](https://www.adafruit.com/product/3309) is about the same price so probably a better option. 

The SM32 is programmed using the Arduino IDE with STM32duino. Make sure to set Optimize to "Fastest (-O3)" in the board options. 

**Note: The pin configurations are temporary and are likely to change between commits and will probably be much different in the "final" version.**

The connections between the serial converter and the STM32 are as follows:

- GND to GND
- 5V to 5V
- RXD to A2
- TXD to A3

The connections for S/NES controller inputs to the STM32 are as follows:

- Player 1 Latch to A1
- Player 1 Clock to A5
- Player 1 Data to A6
- Player 2 Latch to B4
- Player 2 Data to B0
- Player 2 Clock to B7

The connections for Genesis controller inputs to the STM32 are as follows:

 - Player 1 TH (line select) to A0
 - Player 1 Up to A5
 - Player 1 Down to A6
 - Player 1 Left to A7
 - Player 1 Right to A8
 - Player 1 TL (B/A) to A9
 - Player 1 TR (C/Start) to A10
 - Player 2 TH (line select) to B4
 - Player 2 Up to B7
 - Player 2 Down to B0
 - Player 2 Left to B1
 - Player 2 Right to B2
 - Player 2 TL (B/A) to B3
 - Player 2 TR (C/Start) to B6

## Special thanks

I want to give a special thank you to **mantixero** for helping with testing, being excited about the project and cheering me on. Thanks man!
