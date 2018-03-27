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
by sending string "rst" from the arduino and calling ctx.clear()

## Part B. Etch a Sketch
1. Describe which sensors you are using as inputs and why. <br />
2. In what format does the server expect the X & Y values from the Arduino? <br />
3. What else does the Arduino need to do? The sensorCode folder has an almost working example. <br />
4. What range of output X & Y values do you need? Is it better to map the values from the inputs to the outputs in the Arduino code, the Node code or Client code? Why?  <br />
5. How often do you need to be sending data from the Arduino?  <br />
6. Include a copy of the Arduino code, and a copy of your Server and client side code.  <br />

## Part C. Make it yours
1. Upload video of your Etch-a-Sketch in action, being used by someone else in the class!  <br />
