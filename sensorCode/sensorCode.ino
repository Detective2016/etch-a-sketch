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
