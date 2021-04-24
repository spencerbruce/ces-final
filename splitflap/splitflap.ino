// Connect the port of the stepper motor driver
int outPorts[] = {4, 5, 6, 7};

// define parameters for ultrasonic sensor
#define trigPin 2 // define TrigPin
#define echoPin 3 // define EchoPin.
#define MAX_DISTANCE 700 // Maximum sensor distance is rated at 400-500cm.
float timeOut = MAX_DISTANCE * 60; 
int soundVelocity = 340; // define sound speed=340m/s
int past[] = {200,200,200,200,200,200};
bool infront = false;
bool comfy = false;
int waiting = 0;

void setup() {
  // set pins to output
  for (int i = 0; i < 6; i++) {
    pinMode(outPorts[i], OUTPUT);
  }
  pinMode(trigPin,OUTPUT);// set trigPin to output mode
  pinMode(echoPin,INPUT); // set echoPin to input mode
  Serial.begin(115200);   // Open serial monitor at 115200 baud to see ping results.
}

void loop(){
  for (int i = 0; i < 5; i++) { 
    past[i] = past[i+1];
  }
  past[5] = getSonar();
  infront = false;
  for(int i=0;i<6;i++)
    if (past[i] < 100)
      infront = true;
      
  if (infront) {
    waiting = waiting + 1;
    Serial.print("\nin front, waiting ");
    Serial.print(waiting);
    if (waiting > 200) {
      moveAngle(true, 8, 1);
    }
    else if (waiting > 150 && waiting % 2 == 0) {
        moveAngle(true, 4, 1);
    }
    else if (waiting > 100 && waiting % 4 == 0) {
        moveAngle(true, 4, 1);
    }
    delay(100);
  }
  else {
    waiting = 0;
    moveAngle(true, 8, 1);
  }
}

void moveSteps(bool dir, int steps, byte ms) { 
  for (unsigned long i = 0; i < steps; i++) {
    moveOneStep(dir); // Rotate a step
    delay(constrain(ms,3,20));        // Control the speed
  }
}

float getSonar() {
  unsigned long pingTime;
  float distance;
  // make trigPin output high level lasting for 10μs to triger HC_SR04
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Wait HC-SR04 returning to the high level and measure out this waitting time
  pingTime = pulseIn(echoPin, HIGH, timeOut); 
  // calculate the distance according to the time
  distance = (float)pingTime * soundVelocity / 2 / 10000; 
  return distance; // return the distance value
}

void moveOneStep(bool dir) {
  // Define a variable, use four low bit to indicate the state of port 
  static byte out = 0x01;
  // Decide the shift direction according to the rotation direction
  if (dir) { // ring shift left
    out!=0x08? out=out<<1:out=0x01; 
  }
  else { // ring shift right
    out!=0x01? out=out>>1:out=0x08;
  }
  // Output singal to each port
  for (int i = 0; i < 4; i++) {
    digitalWrite(outPorts[i], (out & (0x01 << i)) ? HIGH : LOW);
  } 
}

void moveAround(bool dir, int turns, byte ms){ 
  for(int i=0;i<turns;i++)
    moveSteps(dir,32*64,ms);
}

void moveAngle(bool dir, int angle, byte ms){
  moveSteps(dir,(angle*32*64/360),ms);
}
