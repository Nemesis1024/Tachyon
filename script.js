//****This section is for the serial communication****
const connectButton = document.getElementById("connect-button");
let port;
let writer;
let reader;
let controllerMode = "snes";

//Check if the serial API is enabled and add enable the connect button if it is
if ("serial" in navigator) {
  const notSupported = document.getElementById("notSupported");
  const serialConnect = document.getElementById("serialConnect");
  notSupported.classList.add("hidden");
  serialConnect.classList.remove("hidden");
  connectButton.addEventListener("click", function() {
    if (port) {
      serialDisable();
    } else {
      serialInit();
    }
  });
  connectButton.disabled = false;
}

async function serialInit() {
  port = await navigator.serial.requestPort({});
  await port.open({ baudRate: 115200 });
  console.log("Connected");
  connectButton.innerText = "Connecting... 🔌";
  let status = await shakeHands();
  if (status == true) {
    connectButton.innerText = "Disconnect 🔌";
  } else {
    if (port) {
      serialDisable();
    }
  }
}

async function serialDisable() {
  console.log("Disabling serial");
  if (port) {
    port.close();
    reader = undefined;
    writer = undefined;
    port = undefined;
  }
  console.log("Disconnected");
  connectButton.innerText = "Connect 🔌";
}

async function shakeHands() {
  console.log("Probing...");
  let response = await arduinoCommunicate(65535, 4);
  if (response == true) {
    console.log("A wild Arduino appears");
    if ((controllerMode == "snes") | "nes") {
      response = await arduinoCommunicate(20037, 1); //Tell Arduino we are in S/NES mode ("NE")
    }
    if (controllerMode == "gen3") {
      response = await arduinoCommunicate(18227, 1); //Tell Arduino we are in Genesis 3 button mode ("G3")
    }
    if (controllerMode == "gen6") {
      response = await arduinoCommunicate(18230, 1); //Tell Arduino we are in Genesis 6 button mode ("G6")
    }
    if (response == true) {
      console.log("Arduino says hi");
      response = await arduinoCommunicate(18537, 2); //"Hi"
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
    const message = new Uint16Array([request]);
    const writer = port.writable.getWriter();
    const reader = port.readable.getReader();
    await writer.write(message);
    const { value, done } = await reader.read();
    writer.releaseLock();
    reader.releaseLock();
    if (value == response) {
      return true;
    } else {
      console.error("Was expecting " + response + " but got: " + value);
      return false;
    }
  } else {
    console.error("Not connected to arduino");
  }
}
//****End of serial communication section****

//****This section is for the gamepad input****

let controllers = {};
let controllerTimestamps = {};
let buttonA = {};
let buttonB = {};
let buttonX = {};
let buttonY = {};
let back = {};
let start = {};
let bumperL = {};
let bumperR = {};
let dpadU = {};
let dpadD = {};
let dpadL = {};
let dpadR = {};
let controllerData = [{}, new Uint16Array()];

//Add newly connected gamepads
function gamepadconnected(e) {
  if (e.gamepad) {
    if (e.gamepad.mapping != "standard") {
      console.error("Incompatible gamepad connected - " + e.gamepad.id);
    } else {
      const gamepad = e.gamepad;
      if (!controllers[gamepad.index]) {
        controllers[gamepad.index] = gamepad;
        console.log(gamepad);
      }
      let controllerSvg = document.getElementById(
        "gamepad-" + (0 + gamepad.index)
      ).firstElementChild;
      controllerSvg
        .getElementById("disconnected")
        .setAttribute("display", "none"); //remove disconnected message
      buttonA[gamepad.index] = controllerSvg.getElementById("A");
      buttonB[gamepad.index] = controllerSvg.getElementById("B");
      buttonX[gamepad.index] = controllerSvg.getElementById("X");
      buttonY[gamepad.index] = controllerSvg.getElementById("Y");
      back[gamepad.index] = controllerSvg.getElementById("back");
      start[gamepad.index] = controllerSvg.getElementById("start");
      bumperL[gamepad.index] = controllerSvg.getElementById("lBumper");
      bumperR[gamepad.index] = controllerSvg.getElementById("rBumper");
      dpadU[gamepad.index] = controllerSvg.getElementById("U");
      dpadD[gamepad.index] = controllerSvg.getElementById("D");
      dpadL[gamepad.index] = controllerSvg.getElementById("L");
      dpadR[gamepad.index] = controllerSvg.getElementById("R");
      controllerTimestamps[gamepad.index] = gamepad.timestamp;
      window.requestAnimationFrame(renderGamepad);
    }
  }
}

//Remove disconnected gamepads
function gamepaddisconnected(e) {
  const gamepad = e.gamepad;
  let controllerSvg = document.getElementById("gamepad-" + (0 + gamepad.index))
    .firstElementChild;
  controllerSvg
    .getElementById("disconnected")
    .setAttribute("display", "inherited");
  delete controllers[gamepad.index];
}

//Listen for connected or disconnected gamepads
window.addEventListener("gamepadconnected", gamepadconnected);
window.addEventListener("gamepaddisconnected", gamepaddisconnected);

//Main animation loop
async function renderGamepad() {
  updateGamepads();
  for (let i in controllers) {
    let controller = controllers[i];
    let currentTimestamp = controller.timestamp;
    if (currentTimestamp > controllerTimestamps[i]) {
      let time = performance.now();
      let prevControllerData = controllerData[i];
      controllerTimestamps[i] = currentTimestamp;
      //Set controller ID
      if (i == 0) {
        controllerData[i] = 0b0000000000000000;
      }
      if (i == 1) {
        controllerData[i] = 0b0100000000000000;
      }
      if (i == 2) {
        controllerData[i] = 0b1000000000000000;
      }
      if (i == 3) {
        controllerData[i] = 0b1100000000000000;
      }
      //Capture controller input here and send to SVG and serial
      let buttons = controller.buttons;
      let axes = controller.axes;
      for (let b in buttons) {
        if (b == 0) {
          // A button
          if (buttons[b].pressed) {
            controllerData[i] = controllerData[i] | 0b0000000000000001; // SNES B
            buttonA[i].classList.add("pressed");
          } else {
            buttonA[i].classList.remove("pressed");
          }
        }
        if (b == 1) {
          // B button
          if (buttons[b].pressed) {
            controllerData[i] = controllerData[i] | 0b0000000100000000; // SNES A
            buttonB[i].classList.add("pressed");
          } else {
            buttonB[i].classList.remove("pressed");
          }
        }
        if (b == 2) {
          // X button
          if (buttons[b].pressed) {
            controllerData[i] = controllerData[i] | 0b0000000000000010; // SNES Y
            buttonX[i].classList.add("pressed");
          } else {
            buttonX[i].classList.remove("pressed");
          }
        }
        if (b == 3) {
          // Y button
          if (buttons[b].pressed) {
            controllerData[i] = controllerData[i] | 0b0000001000000000; // SNES X
            buttonY[i].classList.add("pressed");
          } else {
            buttonY[i].classList.remove("pressed");
          }
        }
        if (b == 4) {
          // Left Bumper
          if (buttons[b].pressed) {
            controllerData[i] = controllerData[i] | 0b0000010000000000; // SNES LB
            bumperL[i].classList.add("pressed");
          } else {
            bumperL[i].classList.remove("pressed");
          }
        }
        if (b == 5) {
          // Right Bumper
          if (buttons[b].pressed) {
            controllerData[i] = controllerData[i] | 0b0000100000000000; // SNES RB
            bumperR[i].classList.add("pressed");
          } else {
            bumperR[i].classList.remove("pressed");
          }
        }
        if (b == 8) {
          // Back
          if (buttons[b].pressed) {
            controllerData[i] = controllerData[i] | 0b0000000000000100; // SNES Select
            back[i].classList.add("pressed");
          } else {
            back[i].classList.remove("pressed");
          }
        }
        if (b == 9) {
          // Start
          if (buttons[b].pressed) {
            controllerData[i] = controllerData[i] | 0b0000000000001000; // SNES Start
            start[i].classList.add("pressed");
          } else {
            start[i].classList.remove("pressed");
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
            // Left Joy - Right
            controllerData[i] = controllerData[i] | 0b0000000010000000; // SNES Right
            dpadR[i].classList.add("pressed");
          } else {
            dpadR[i].classList.remove("pressed");
          }
          if (axes[a] < -0.45 || buttons[14].pressed) {
            // Left Joy - Left
            controllerData[i] = controllerData[i] | 0b0000000001000000; // SNES Left
            dpadL[i].classList.add("pressed");
          } else {
            dpadL[i].classList.remove("pressed");
          }
        }
        if (a == 1) {
          if (axes[a] > 0.45 || buttons[13].pressed) {
            // Left Joy - Down
            controllerData[i] = controllerData[i] | 0b0000000000100000; // SNES Down
            dpadD[i].classList.add("pressed");
          } else {
            dpadD[i].classList.remove("pressed");
          }
          if (axes[a] < -0.45 || buttons[12].pressed) {
            // Left Joy - Up
            controllerData[i] = controllerData[i] | 0b0000000000010000; // SNES Up
            dpadU[i].classList.add("pressed");
          } else {
            dpadU[i].classList.remove("pressed");
          }
        }
      }
      if (prevControllerData != controllerData[i]) {
        let ack = await arduinoCommunicate(controllerData[i], 1);
        if (ack == false) {
          console.error("Serial communication error, attempting reset");
          if (port) {
            let status = await shakeHands();
            if (status != true) {
              await serialDisable();
            }
          }
        }
        console.log(
          "Processing time - " +
            Math.trunc((performance.now() - time) * 1000) +
            " μs"
        );
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

//****Configuration Functions****

async function changeSkin(skin) {
  for (const child of document.getElementById("consoleSelect").children) {
    child.children[0].classList.remove("active");
  }
  document.getElementById(skin).classList.add("active");

  let controllerDIV = document.getElementsByClassName("controller");
  for (let i = 0; i < controllerDIV.length; i++) {
    let img = document.createElement("img");
    img.src = skin + "/" + skin + ".svg";
    controllerDIV[i].firstElementChild.remove();
    controllerDIV[i].appendChild(img);
  }

  SVGInject(controllerDIV[0].parentElement.querySelectorAll("img", "svg"), {
    onAllFinish: function() {
      for (let i in controllers) {
        let e = new Object();
        e.gamepad = controllers[i];
        gamepadconnected(e);
      }
    }
  });
  controllerMode = skin;
  if (port) {
    let status = await shakeHands();
    if (status != true) {
      serialDisable();
    }
  }
}

function changePlayerNum(num) {
  for (const child of document.getElementById("numPlayers").children) {
    child.children[0].classList.remove("active");
  }
  document.getElementById(num).classList.add("active");

  if (num == "1") {
    document.getElementById("gamepad-0").classList =
      "controller controllerSingle";
    document.getElementById("gamepad-1").classList = "controller hidden";
    document.getElementById("gamepad-2").classList = "controller hidden";
    document.getElementById("gamepad-3").classList = "controller hidden";
  }
  if (num == "2") {
    document.getElementById("gamepad-0").classList =
      "controller controllerLeft";
    document.getElementById("gamepad-1").classList =
      "controller controllerRight";
    document.getElementById("gamepad-2").classList = "controller hidden";
    document.getElementById("gamepad-3").classList = "controller hidden";
  }
  if (num == "3") {
    document.getElementById("gamepad-0").classList =
      "controller controllerCenter";
    document.getElementById("gamepad-1").classList =
      "controller controllerLeft";
    document.getElementById("gamepad-2").classList =
      "controller controllerRight";
    document.getElementById("gamepad-3").classList = "controller hidden";
  }
  if (num == "4") {
    document.getElementById("gamepad-0").classList =
      "controller controllerLeft";
    document.getElementById("gamepad-1").classList =
      "controller controllerRight";
    document.getElementById("gamepad-2").classList =
      "controller controllerLeft";
    document.getElementById("gamepad-3").classList =
      "controller controllerRight";
  }
}

function changeColor(color) {
  document.querySelectorAll("html")[0].style.backgroundColor = color;
}
