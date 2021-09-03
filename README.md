# Web Serial to Arduino Gamepad

Use your web browser (currently Chrome version 78 or newer only) to capture game pad inputs and forward them to an Arduino over Serial.
The Arduino sketch then sends the inputs to a classic video game console (currently NES & SNES are supported) or USB via DInput (coming soon).
Support for up to 4 game pads in and out (coming soon).

The main goal for this project is to allow people to play games over [Parsec](https://parsec.app) on real consoles. 
Parsec passes external player controllers through to the host machine as virtual XInput (Xbox 360) controllers. 
This will capture the inputs from those controllers and forward them on to the target console, emulating a native controller.
You will obviously need a **low latency** capture card to capture the video from the console on the Parsec host machine. I use a Datapath Vision E1 for this. 

The web interface also provides a controller input display for all of the controllers with skins for each supported output console. 
This part was heavily inspired by [Gamepad Viewer](https://gamepadviewer.com) by mrmcpowned but has been written mostly from scratch.

Try it out here: [https://nemesis1024.github.io/web-serial-arduino-gamepad/](https://nemesis1024.github.io/web-serial-arduino-gamepad/)

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
- Genesis/Mega Drive output and skin
- PSX output and skin
- N64 output and skin
- USB output and skin
- Multitap emulation
- Controller to controller remapping (maybe)
- Button remapping (maybe)
- Additional console support
- A neat little hardware extra ;)

## Further information

The hardware uses a [5V/16MHz Arduino Pro Micro](https://www.sparkfun.com/products/12640) and a [generic CP2104 USB to Serial Converter](https://www.amazon.com/NOYITO-CP2104-Serial-Converter-Adapter/dp/B07F5FPQH) 
but the one [from Adafruit](https://www.adafruit.com/product/3309) is about the same price so probably a better option. 

The connections between the serial converter and the Arduino are as follows:

- GND to GND
- 5V to RAW
- RXD to TX0
- TXD to RX1

The connections for S/NES controllers to the Arduino are as follows:

- Latch (both player 1 & 2) to pin 2
- Player 1 Clock to pin 3
- Player 1 Data to pin 4
- Player 2 Data to pin 5
- Player 2 Clock to pin 6

