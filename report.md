# Lab 6: Make an Etch-a-Sketch!

## Part A. Node Canvas
1. Where in this code does the drawing occur? <br />
drawing occurs in client.js where the new cursor positions are read and updated:
```javascript
ctx.moveTo( previousPosition[0], previousPosition[1] ); // from
ctx.lineTo( newPosition[0],  newPosition[1]); // to
ctx.stroke(); // and only draw a stroke
previousPosition=newPosition; // update to the new position.
```
2. What are the inputs to the drawing function? <br />
inputs are color, x, y, and clear operation signals <br />
3. How can the screen be cleared? <br />
by sending string "rst" from the arduino and calling ctx.clear() <br />

## Part B. Etch a Sketch
1. Describe which sensors you are using as inputs and why. <br />
I'm using potentiometers as my controllers for x and y inputs. For clearing the screen, I'm using a button. When the button is pressed, it should send a clear signal event. <br />
2. In what format does the server expect the X & Y values from the Arduino? <br />
X and Y values are string separated by a comma. <br />
3. What else does the Arduino need to do? The sensorCode folder has an almost working example. <br />
The Arduino needs to do the following in order to be fully functional with the sensors I'm using: <br />
..* the Arduino needs to begin the serial connection with a baudrate of 115200 <br />
..* read sensor values from the button and the two potentiometers <br />
..* add the condition such that when the button is pressed, a string of "rst" is sent <br />
..* if any of the potentiometers' values changed, send a string of x and y delimited by a comma <br />
4. What range of output X & Y values do you need? Is it better to map the values from the inputs to the outputs in the Arduino code, the Node code or Client code? Why? <br />
The X and Y values need to be mapped to the container borders using styles and client code. It is easier to see and test the changes since the client code passes these values to the html for display. Additionally, since there are limits to what the potentiometers can rotate, I've used that as a reference when setting the range of the outputs. <br />
5. How often do you need to be sending data from the Arduino? <br />
As often as there's a change in the x or y values, or if there's a clear signal (when the button is pressed). <br />
6. Include a copy of the Arduino code, and a copy of your Server and client side code. <br />
Arduino:  <br />
```c
#define SENSORPINA A0 // x axis
 //TODO: define other sensor inputs
const int RESET = 2;
const int X_CTRL = A0;
const int Y_CTRL = A1;
int c = 0;
int x = 0;
int y = 0;
unsigned long targetTime=0;
const unsigned long interval=1500; //TODO: How fast should we read

void setup(){
  // TODO: begin the serial connection with a baudrate of 115200
  Serial.begin(115200);
  pinMode(X_CTRL, INPUT);
  pinMode(Y_CTRL, INPUT);
  pinMode(RESET, INPUT);
  //Serial.begin(9600);
}

void loop(){
  if(millis()>=targetTime){
    targetTime= millis()+interval;
    // Serial.println(analogRead(SENSORPINA));

    // TODO: Add other sensor read outs
    c = digitalRead(RESET);
    int xn = analogRead(X_CTRL);
    int yn = analogRead(Y_CTRL);
    
    // TODO: convert values into a string https://www.arduino.cc/en/Tutorial/StringConstructors
    // TODO: combine them into a string that can be understood by server.js
    // TODO: send the string over serial
    // TODO: Detect if you want to reset the screen(shake the etch-a-sketch)
    // TODO: write the reset message(see server.js) to the serial port
    if (c == HIGH) {
      x = 0;
      y = 0;
      Serial.println("rst");
    } else if (xn != x || yn != y) {
      x = xn;
      y = yn;
      Serial.println(String(x) + "," + String(y));
    }
  }
}
```
Server: <br />
```javascript
var express = require('express'); // web server application
var app = express(); // webapp
var http = require('http').Server(app); // connects http library to server
var io = require('socket.io')(http); // connect websocket library to server
var serverPort = 8000;
var serialPort = require('serialport'); // serial library
var readLine = serialPort.parsers.Readline; // read serial data as lines

//---------------------- WEBAPP SERVER SETUP ---------------------------------//
// use express to create the simple webapp
app.use(express.static('public')); // find pages in public directory


// start the server and say what port it is on
http.listen(serverPort, function() {
  console.log('listening on *:%s', serverPort);
});
//----------------------------------------------------------------------------//


//---------------------- SERIAL COMMUNICATION --------------------------------//
// start the serial port connection and read on newlines
const serial = new serialPort('/dev/ttyUSB0', {
 baudRate:115200

});
const parser = new readLine({
  delimiter: '\r\n'
});

// Read data that is available on the serial port and send it to the websocket
serial.pipe(parser);
parser.on('data', function(data) { // on data from the arduino
  if(data=='rst'){  // if its the 'rst' string call reset
    io.emit('reset');
  }else{ // any other data we try to forward by spliting it
    var transmitData = [data.split(',')[0],data.split(',')[1]];
    io.emit('new-pos', transmitData);
  }
});
//----------------------------------------------------------------------------//


//---------------------- WEBSOCKET COMMUNICATION -----------------------------//
// this is the websocket event handler and say if someone connects
// as long as someone is connected, listen for messages
io.on('connect', function(socket) {
  console.log('a user connected');
  io.emit('reset'); // call reset to make sure the website is clean

// if you get the 'disconnect' message, say the user disconnected
  io.on('disconnect', function() {
    console.log('user disconnected');
  });
});
```
Client: <br />
```javascript
var COLOUR =  '#505050';  // This is the drawing color
var radius = 3;           // Constant radio for the line
var socket = io();        // websocket to the server
var previousPosition=[0,0]; // previous position to draw a line from
var ctx = Sketch.create({
    container: document.getElementById( 'container' ), //reference drawing canvas
    autoclear: false, // making sure it stays
    retina: 'auto',
}); //Creating the drawing context
var firstMessage=true;    // What the first message, to start on the first value

    ctx.setup = function() { console.log( 'setup' );} // Setup all variables
    ctx.keydown= function() { if ( ctx.keys.C ) ctx.clear();} // handeling keydowns

    socket.on('reset', function() { // on a 'reset' message clean and reste firstMessage
      firstMessage=true;
      ctx.clear();
    });

    socket.on('new-pos', function(newPosition) { // handling new sensor values

      //TODO: Map the incoming 10-bit numbers to the height and width of the screen.
      // See https://github.com/soulwire/sketch.js/wiki/API for sketch references
      newPosition[0] = map(newPosition[0], 0, 1023, 0, ctx.width*.6);
      newPosition[1] = map(newPosition[1], 0, 1023, 0, ctx.height*.6);

      if(firstMessage){ // if its the first message store that value as previous
        firstMessage=false;
        previousPosition=newPosition;
      }else{ // any other message we use to draw.
        ctx.lineCap = 'round';
        ctx.lineJoin = 'round';
        ctx.fillStyle = ctx.strokeStyle = COLOUR;
        ctx.lineWidth = radius;
        ctx.beginPath();  //begin a adrawing
        ctx.moveTo( previousPosition[0], previousPosition[1] ); // from
        ctx.lineTo( newPosition[0],  newPosition[1]); // to
        ctx.stroke(); // and only draw a stroke
        previousPosition=newPosition; // update to the new position.
       }
    });
    
    $('select[name="colorpicker"]').on('change', function() {
      COLOUR = $('select[name="colorpicker"]').val();
    });
```
HTML: <br />
```html
<html>
  <head>
    <script src="/socket.io/socket.io.js"></script>
    <script src="sketch.min.js" type="text/javascript"></script>
    <link href="style.css" rel="stylesheet">
    <script src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.js"></script>
    <title>YM Lab6 Etch A Sketch</title>
  </head>

  <body>
    <div id="header">
      <h1>My Sketch</h1>
        <p>color selection:</p>
        <select name="colorpicker">
        <option value="#000000">Black</option>
        <option value="#7bd148">Green</option>
        <option value="#5484ed">Bold blue</option>
        <option value="#a4bdfc">Blue</option>
        <option value="#46d6db">Turquoise</option>
        <option value="#7ae7bf">Light green</option>
        <option value="#51b749">Bold green</option>
        <option value="#fbd75b">Yellow</option>
        <option value="#ffb878">Orange</option>
        <option value="#ff887c">Red</option>
        <option value="#dc2127">Bold red</option>
        <option value="#dbadff">Purple</option>
        <option value="#e1e1e1">Gray</option>
      </select>
    </div>
    <div id="container"></div>
    <div id="footer">
      <h2>etch-a-sketch</h2>
    </div>
    <script src ="client.js" type="text/javascript"> </script>
  </body>

</html>
```
Styles: <br />
```css
/* Styles go here */

html, body {
    background: #EEEEEE;
}

#container {
  width: 1200px;
  height: 600px;
  border: 2px solid blue;
}
```

## Part C. Make it yours
1. Upload video of your Etch-a-Sketch in action, being used by someone else in the class! <br />

### References
https://github.com/tkrotoff/jquery-simplecolorpicker
