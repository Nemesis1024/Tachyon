"use strict";
class SerialGameController {
  constructor() {
    this.dataArray = new Uint8Array(2);
    this.resetCommand = new Uint8Array([255]);
    this.arduinoGreeting = new Uint8Array([253, 216]);
  }
  async init() {
    if ("serial" in navigator) {
      try {
        const port = await navigator.serial.requestPort();
        await port.open({ baudrate: 115200 });
        this.reader = port.readable.getReader();
        this.writer = port.writable.getWriter();
        console.log("Connected");
      } catch (err) {
        console.error("There was an error opening the serial port:", err);
      }
      // if (this.writer != null) {
      //   await this.writer.write(this.resetCommand);
      //   try {
      //     const readerData = await this.reader.read();
      //     if (readerData.value == 4) {
      //       console.log("Arduino reset");
      //     }
      //   } catch (err) {
      //     const errorMessage = `error reading data: ${err}`;
      //     console.error(errorMessage);
      //     return errorMessage;
      //   }
      // } else {
      //   console.error("Not connected to a serial port.");
      // }
    } else {
      console.error(
        "Web serial doesn't seem to be enabled in your browser. Try enabling it (if Chrome, visit chrome://flags/#enable-experimental-web-platform-features)."
      );
    }
  }

  async write(data) {
    this.dataArray[0] = data;
    return await this.writer.write(this.dataArray);
  }
  async read() {
    try {
      const readerData = await this.reader.read();
      return readerData.value;
    } catch (err) {
      const errorMessage = `error reading data: ${err}`;
      console.error(errorMessage);
      return errorMessage;
    }
  }
}
