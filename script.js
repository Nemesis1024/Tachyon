const connectButton = document.getElementById("connect-button");
//const shakeHandsButton = document.getElementById("shake-hands-button");
let port;
let writer;
let reader;
//let inputBuffer = 0;

if ("serial" in navigator) {
  connectButton.addEventListener("click", function() {
    if (port) {
      reader.cancel();
      writer.releaseLock();
      port.close();
      port = undefined;
      console.log("Disconnected")
      connectButton.innerText = "🔌 Connect";
    } else {
      serialInit();
    }
  });
  connectButton.disabled = false;
}

// shakeHandsButton.addEventListener("click", function() {
//   shakeHands();
// });
// shakeHandsButton.disabled = false;

async function serialInit() {
  port = await navigator.serial.requestPort({});
  await port.open({ baudrate: 115200 });
  connectButton.innerText = "🔌 Disconnect";
  writer = port.writable.getWriter();
  reader = port.readable.getReader();
  console.log("Connected");
  shakeHands();
}

async function serialWrite(data) {
  try {
    await writer.writable;
    await writer.write(data);
    return data;
  } catch (err) {
    console.error(err);
  }
}

async function serialRead() {
  try {
    await reader.readable;
    const data = await reader.read();
    //console.log(data);
    return data.value;
  } catch (err) {
    console.error(err);
  }
}

function shakeHands() {
  let response = arduinoCommunicate(255, 4);
  if (response) {
  console.log ("A wild Arduino appears");
  } else {
    console.error ("Nothing happened");
  }
  response = arduinoCommunicate(253, 1);
  if (response) {
    console.log ("Arduino says hi")
  } else {
    console.error ("Arduino has run away")
  }
  response = arduinoCommunicate(216, 2);
  if (response) {
    console.log ("Arduino and computer are now friends")
  } else {
    console.error ("Arduino has left the party")
  }
}

async function arduinoCommunicate(request, response) {
  let bytes = new Uint8Array([request]); //Initilize with reset command
  let arduinoResponse = new Uint8Array(1);

  await serialWrite(bytes); //Send command and wait for it to finish
  arduinoResponse = await serialRead(); //Wait for reply from Arduino
  //console.log(arduinoResponse);
  if (arduinoResponse == response) {
    return(true);
  } else {
    return(false);
  }
}

function renderGamepad() {
  window.requestAnimationFrame(renderGamepad);
}
window.requestAnimationFrame(renderGamepad);
