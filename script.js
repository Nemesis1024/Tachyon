//****This section is for the serial communication****
const connectButton = document.getElementById("connect-button");
let port;
let writer;
let reader;
let arduinoReady = false;
// let serialResponse;
// let greeting = new Uint8Array([255, 253, 216]);

//Check if the serial API is enabled and add enable the connect button if it is
if ("serial" in navigator) {
  const notSupported = document.getElementById("notSupported");
  const serialConnect = document.getElementById("serialConnect");
  notSupported.classList.add("hidden");
  serialConnect.classList.remove("hidden");
  connectButton.addEventListener("click", function() {
    if (port) {
      reader.cancel();
      writer.releaseLock();
      port.close();
      port = undefined;
      console.log("Disconnected");
      connectButton.innerText = "🔌 Connect";
    } else {
      serialInit();
    }
  });
  connectButton.disabled = false;
}

async function serialInit() {
  port = await navigator.serial.requestPort({});
  await port.open({ baudrate: 115200 });
  writer = port.writable.getWriter();
  reader = port.readable.getReader();
  console.log("Connected");
  connectButton.innerText = "🔌 Connecting...";
  // serialInputHandler();
  let status = await shakeHands();
  //console.log("Shake hands: " + status);
  if (status == true) {
    connectButton.innerText = "🔌 Disconnect";
    // serialInputHandler();
  } else {
    if (port) {
      reader.cancel();
      writer.releaseLock();
      port.close();
      port = undefined;
      reader = undefined;
      writer = undefined;
      console.log("Disconnected");
    }
  }
}

async function serialWrite(data) {
  try {
    await writer.writable;
    //console.log("Serial write: " + data);
    return await writer.write(data);
    //console.log (response)
    //return response;
  } catch (err) {
    console.error(err);
  }
}

async function serialRead() {
  try {
    // let readable = await reader.readable();
    let data = await reader.read();
    // console.log(data);
    return data.value;
  } catch (err) {
    console.error(err);
  }
}

async function wait(ms) {
  return new Promise(resolve => {
    setTimeout(resolve, ms);
  });
}

async function shakeHands() {
  let response;
  response = await serialWrite(new Uint8Array([255]));
  response = await serialWrite(new Uint8Array([255]));
  // await wait(0.1);
  response = await serialRead();
  // console.log("Reset? " + response);
  response = await arduinoCommunicate(255, 4);
  // console.log("Response: " + response);
  if (response == true) {
    console.log("A wild Arduino appears");
    response = await arduinoCommunicate(253, 1);
    // console.log("Response: " + response);
    if (response == true) {
      console.log("Arduino says hi");
      response = await arduinoCommunicate(216, 2);
      // console.log("Response: " + response);
      if (response == true) {
        console.log("Arduino and computer are now friends");
      } else {
        console.error("Arduino has left the party");
      }
    } else {
      console.error("Arduino has run away");
    }
  } else {
    console.error("Nothing happened");
  }
  return response;
}

async function arduinoCommunicate(request, response) {
  if (port) {
    let bytes = new Uint8Array([request]);
    let arduinoResponse; // = new Uint8Array(1);
    // console.log(bytes);
    // serialResponse = undefined;
    let write = await serialWrite(bytes); //Send command and wait for it to finish
    // console.log("write complete");
    arduinoResponse = await serialRead(); //Wait for reply from Arduino
    // console.log("Arduino says: " + arduinoResponse);
    console.log("Got: " + arduinoResponse + " Expected: " + response);
    if (arduinoResponse == response) {
      return true;
    } else {
      return false;
    }
  } else {
    console.error("Not connected to Arduino");
  }
}
//****End of serial communication section****

//****This section is for the gamepad input****

//!!!!Need to add player and button remapping functions!!!!

let controllers = {};
let controllerTimestamps = {};
let button = {};
let back = {};
let start = {};
let bumper = {};
let dpad = {};
let controllerData = [{}, new Uint8Array(2)];

//Add newly connected gamepads
function gamepadconnected(e) {
  const gamepad = e.gamepad;
  controllers[gamepad.index] = gamepad;
  let controllerDiv = document.getElementById("gamepad-0"); // + gamepad.index);
  controllerDiv.classList.remove("disconnected");
  button[gamepad.index] = controllerDiv.getElementsByClassName("button");
  back[gamepad.index] = controllerDiv.getElementsByClassName("back");
  start[gamepad.index] = controllerDiv.getElementsByClassName("start");
  bumper[gamepad.index] = controllerDiv.getElementsByClassName("bumper");
  dpad[gamepad.index] = controllerDiv.getElementsByClassName("face");
  console.log(button[gamepad.index]);
  console.log(gamepad);
  controllerTimestamps[gamepad.index] = gamepad.timestamp;
  window.requestAnimationFrame(renderGamepad);
}

//Remove disconnected gamepads
function gamepaddisconnected(e) {
  const gamepad = e.gamepad;
  let controllerDiv = document.getElementById("gamepad-0"); // + gamepad.index);
  controllerDiv.classList.add("disconnected");
  delete controllers[gamepad.index];
}

//Listen for connected or disconnected gamepads
window.addEventListener("gamepadconnected", gamepadconnected);
window.addEventListener("gamepaddisconnected", gamepaddisconnected);

//Main animation loop
function renderGamepad() {
  updateGamepads();
  for (let i in controllers) {
    let controller = controllers[i];
    let currentTimestamp = controller.timestamp;
    if (currentTimestamp > controllerTimestamps[i]) {
      let time = performance.now();
      let prevControllerData = $.extend(true, {}, controllerData[i]);
      controllerTimestamps[i] = currentTimestamp;
      //Set serial controller ID and initilize first byte
      if (i == 0) {
        controllerData[i][0] = 0b11000000;
      }
      if (i == 1) {
        controllerData[i][0] = 0b01000000;
      }
      if (i == 2) {
        controllerData[i][0] = 0b10000000;
      }
      if (i == 3) {
        controllerData[i][0] = 0b00000000;
      }
      controllerData[i][1] = 0b00000000;
      //Capture controller input here and send to HTML and serial
      let buttons = controller.buttons;
      let axes = controller.axes;
      for (let b in buttons) {
        if (b == 0) {
          if (buttons[b].pressed) {
            controllerData[i][1] = controllerData[i][1] | 0b00000001;
            button[i][0].classList.add("pressed");
          } else {
            button[i][0].classList.remove("pressed");
          }
        }
        if (b == 1) {
          if (buttons[b].pressed) {
            controllerData[i][1] = controllerData[i][1] | 0b00000010;
            button[i][1].classList.add("pressed");
          } else {
            button[i][1].classList.remove("pressed");
          }
        }
        if (b == 2) {
          if (buttons[b].pressed) {
            controllerData[i][1] = controllerData[i][1] | 0b00000100;
            button[i][2].classList.add("pressed");
          } else {
            button[i][2].classList.remove("pressed");
          }
        }
        if (b == 3) {
          if (buttons[b].pressed) {
            controllerData[i][1] = controllerData[i][1] | 0b00001000;
            button[i][3].classList.add("pressed");
          } else {
            button[i][3].classList.remove("pressed");
          }
        }
        if (b == 4) {
          if (buttons[b].pressed) {
            controllerData[i][1] = controllerData[i][1] | 0b00010000;
            bumper[i][0].classList.add("pressed");
          } else {
            bumper[i][0].classList.remove("pressed");
          }
        }
        if (b == 5) {
          if (buttons[b].pressed) {
            controllerData[i][1] = controllerData[i][1] | 0b00100000;
            bumper[i][1].classList.add("pressed");
          } else {
            bumper[i][1].classList.remove("pressed");
          }
        }
        if (b == 8) {
          if (buttons[b].pressed) {
            controllerData[i][1] = controllerData[i][1] | 0b01000000;
            back[i][0].classList.add("pressed");
          } else {
            back[i][0].classList.remove("pressed");
          }
        }
        if (b == 9) {
          if (buttons[b].pressed) {
            controllerData[i][1] = controllerData[i][1] | 0b10000000;
            start[i][0].classList.add("pressed");
          } else {
            start[i][0].classList.remove("pressed");
          }
        }

        // if (b == 12) {
        //   if (buttons[b].pressed) {
        //     controllerData[i][1] = controllerData[i][1] | 0b10000000;
        //     dpad[i][0].classList.add("pressed");
        //   } else {
        //     dpad[i][0].classList.remove("pressed");
        //   }
        // }
        // if (b == 13) {
        //   if (buttons[b].pressed) {
        //     controllerData[i][1] = controllerData[i][1] | 0b10000000;
        //     dpad[i][1].classList.add("pressed");
        //   } else {
        //     dpad[i][1].classList.remove("pressed");
        //   }
        // }
        // if (b == 14) {
        //   if (buttons[b].pressed) {
        //     controllerData[i][1] = controllerData[i][1] | 0b10000000;
        //     dpad[i][2].classList.add("pressed");
        //   } else {
        //     dpad[i][2].classList.remove("pressed");
        //   }
        // }
        // if (b == 15) {
        //   if (buttons[b].pressed) {
        //     controllerData[i][1] = controllerData[i][1] | 0b10000000;
        //     dpad[i][3].classList.add("pressed");
        //   } else {
        //     dpad[i][3].classList.remove("pressed");
        //   }
        // }
      }
      for (let a in axes) {
        if (a == 0) {
          if (axes[a] > 0.45 || buttons[15].pressed) {
            controllerData[i][0] = controllerData[i][0] | 0b00001000;
            dpad[i][3].classList.add("pressed");
          } else {
            dpad[i][3].classList.remove("pressed");
          }
          if (axes[a] < -0.45 || buttons[14].pressed) {
            controllerData[i][0] = controllerData[i][0] | 0b00000100;
            dpad[i][2].classList.add("pressed");
          } else {
            dpad[i][2].classList.remove("pressed");
          }
        }
        if (a == 1) {
          if (axes[a] > 0.45 || buttons[13].pressed) {
            controllerData[i][0] = controllerData[i][0] | 0b00100000;
            dpad[i][1].classList.add("pressed");
          } else {
            dpad[i][1].classList.remove("pressed");
          }
          if (axes[a] < -0.45 || buttons[12].pressed) {
            controllerData[i][0] = controllerData[i][0] | 0b00010000;
            dpad[i][0].classList.add("pressed");
          } else {
            dpad[i][0].classList.remove("pressed");
          }
        }
      }
      // console.log("Current : " + controllerData[i][0] + ":" + controllerData[i][1]);
      // console.log("Previous: " + prevControllerData[0] +  ":" + prevControllerData[1]);
      if (
        prevControllerData[0] != controllerData[i][0] ||
        prevControllerData[1] != controllerData[i][1]
      ) {
        // console.log("New Data");
        (async () => {
          //***Add await serial.read() here with a timeout, increment frame count and proceed if recieve "3"
          await serialWrite(new Uint8Array([controllerData[i][1]]));
          let ack = await arduinoCommunicate(controllerData[i][0], 1);
          //console.log(ack);
          if (ack == false) {
            console.error("Serial communication error, attempting reset");
            await serialWrite(new Uint8Array([255]));
            await serialWrite(new Uint8Array([255]));
            await shakeHands();
          }
        })();
        console.log(performance.now()- time);
      }
    }
  }
  window.requestAnimationFrame(renderGamepad);
}

//Update the current state of the conencted gamepads
function updateGamepads() {
  let gamepads = navigator.getGamepads();
  for (let i = 0; i < gamepads.length; i++) {
    if (gamepads[i]) {
      if (!(gamepads[i].index in controllers)) {
        gamepadconnected(gamepads[i]);
      } else {
        controllers[gamepads[i].index] = gamepads[i];
      }
    }
  }
}

window.requestAnimationFrame(renderGamepad);
//****End of gamepad input section****
