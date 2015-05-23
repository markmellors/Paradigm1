const int EnableRCPin = 1; //radio control input pin,'throttle' used to decide when to override the autonomous mode
const int EnableLevel = 1700; //cut in level for enable channel
const int LeftRCPin = 2; //radio control input pin 'elevator', used for forward/reverse
const int RightRCPin = 3; //radio control input pin, 'aileron', used for steering
const int LifterRCPin = 4; //radio control input pin, 'Rudder', used for steering
const int LeftESCPin = 23; //
const int RightESCPin = 20; //
const int LifterESCPin = 17; //
const int LifterPotPin = 15; //
int LeftEdgeSensorPin = 14;
int RightEdgeSensorPin = 16; 
int AutoLEDPin = 11; 
int RightFullForward =1200;
int RightForward =1400;
int RightStopped =1480;
int RightReverse =1700;
int RightFullReverse = 1900;
int LeftFullForward =1200;
int LeftForward =1400;
int LeftStopped =1480;
int LeftReverse =1700;
int LeftFullReverse = 1900;
int BrightnessLimit=800;
int StuckCase;
#include <Servo.h> 
Servo LeftESC;
Servo RightESC;
Servo LifterESC;
long LastTriggered;
float Timeout=5000;
long EndTime;
long LastAuto=0;
int EnableSignal=1500, RightSignal=1500, LeftSignal=1500, LifterSignal=1500;
int LifterPosition = 0;        // value read from the pot

void setup(){
  LeftESC.attach(LeftESCPin);
  RightESC.attach(RightESCPin);
  LifterESC.attach(LifterESCPin);
  Serial.begin(9600);
  LastTriggered=millis();
  StuckCase=0;
  pinMode(LeftRCPin,INPUT);
  pinMode(RightRCPin,INPUT);
  pinMode(EnableRCPin,INPUT);
  pinMode(LifterRCPin,INPUT);
  pinMode(AutoLEDPin, OUTPUT); 
}

void loop(){
 
  EnableSignal = pulseIn(EnableRCPin, HIGH, 25000);
  if (EnableSignal > EnableLevel){
    digitalWrite(AutoLEDPin, HIGH);
    Serial.print("auto mode, ");
    if((LastAuto+5000)<millis()){
      LastTriggered=millis();
      FullForward();
      delay(230); 
      AllStop();
      delay(10); 
    }
    LastAuto=millis();
    int LeftEdgeSensor = analogRead(LeftEdgeSensorPin);
    int RightEdgeSensor = analogRead(RightEdgeSensorPin);
    if ((LeftEdgeSensor<BrightnessLimit) && (RightEdgeSensor<BrightnessLimit)) {
      Serial.print("no edge detected, ");
      EndTime = millis()+35;
      while((LeftEdgeSensor<BrightnessLimit) && (RightEdgeSensor<BrightnessLimit) && millis()<EndTime) {
        LeftEdgeSensor = analogRead(LeftEdgeSensorPin);
        RightEdgeSensor = analogRead(RightEdgeSensorPin);
        FullForward();
        delay(10); 
      }
      if ((LeftEdgeSensor<BrightnessLimit) && (RightEdgeSensor<BrightnessLimit)) {
        AllStop();
        delay(70);
      } else {
        FullReverse();
        delay(40);
      }  
    } else { 
      Serial.print("edge detected, ");
      LastTriggered=millis();
      FullReverse();
      delay(30);
      if (LeftEdgeSensor<BrightnessLimit) {
        SpinRight();
      } else {
        SpinLeft();
      }
      delay(200);
      AllStop();
      delay(10);
    }
    if (LastTriggered+Timeout<millis()) {     
      Serial.print("stuck, ");
      StuckCase = StuckCase+1;
      LastTriggered=millis();
      ScoopLift();
    }
    Serial.print(LeftEdgeSensor);
    Serial.print(" ");
    Serial.println(RightEdgeSensor);
    delay(50);
  } else {
    if (EnableSignal > 1400){
       digitalWrite(AutoLEDPin, LOW);
       Debug();         
     } else {
       digitalWrite(AutoLEDPin, LOW);
       RCMode();
     }
  }
}

void TurnRight(){
  Serial.print("turning right, ");
  RightESC.write(RightReverse);
  LeftESC.write(LeftForward);
}
void TurnLeft(){
  Serial.print("turning left, ");
  RightESC.write(RightForward);
  LeftESC.write(LeftReverse);
}
void SpinLeft(){
  Serial.print("spinning left, ");
  RightESC.write(RightFullForward);
  LeftESC.write(LeftFullReverse);
}
void SpinRight(){
  Serial.print("spinning right, ");
  RightESC.write(RightFullReverse);
  LeftESC.write(LeftFullForward);
}
void MoveForward(){
  Serial.print("forward, ");
  LeftESC.write(LeftForward);
  RightESC.write(RightForward);
}
void MoveReverse(){
  Serial.print("reversing, ");
  LeftESC.write(LeftReverse);
  RightESC.write(RightReverse);

}
void FullForward(){
  Serial.print("ramming forwards, ");
  RightESC.write(RightFullForward);
  LeftESC.write(LeftFullForward);    
}

void FullReverse(){
 Serial.print("fast reverse, ");
 RightESC.write(RightFullReverse);
 LeftESC.write(LeftFullReverse);    
}

void AllStop(){
  Serial.print("Stopping, ");
  RightESC.write(RightStopped);
  LeftESC.write(LeftStopped);
}

void Debug() {
      int sensorValue=0;
      Serial.print("Debug Mode v0.8, ");
      sensorValue = analogRead(14);         
      Serial.print("14, left edge sensor: " );                       
      Serial.print(sensorValue);
      sensorValue = analogRead(15);            
      Serial.print(" 15, potentiometer: " );                       
      Serial.print(sensorValue); 
      sensorValue = analogRead(10);            
      Serial.print(" 10, distance sensor: " );                       
      Serial.print(sensorValue); 
      sensorValue = analogRead(16);            
      Serial.print(" 16, right edge sensor: " );                       
      Serial.println(sensorValue);   
      delay(100);
}
void RCMode(){
       Serial.print("RCMode v0.8");
       LeftSignal = pulseIn(LeftRCPin, HIGH, 25000); // Read the pulse width of 
       RightSignal = pulseIn(RightRCPin, HIGH, 25000); // each channel
       LifterSignal =pulseIn(LifterRCPin, HIGH, 25000);
       if (LeftSignal<800) LeftSignal=1500;
       if (RightSignal<800) RightSignal=1500;
       if (LifterSignal<800) LifterSignal=1500;
       Serial.print(", Left input: ");
       Serial.print(LeftSignal);
       Serial.print(", Right input: ");
       Serial.print(RightSignal);  
       Serial.print(", Left outut: ");
       Serial.print(constrain((-LeftSignal+RightSignal)/2+1500,1000,2000));
       Serial.print(", Right outut: ");
       Serial.print(constrain((LeftSignal+RightSignal)/2,1000,2000)); 
       RightESC.write(constrain((LeftSignal+RightSignal)/2,1000,2000));
       LeftESC.write(constrain((-LeftSignal+RightSignal)/2+1500,1000,2000));
       LifterPosition = analogRead(LifterPotPin);
       Serial.print(", lifter command: "); 
       Serial.print(LifterSignal); 
       if (LifterPosition>900){
          LifterSignal= max(1500,LifterSignal);
       }
       if (LifterPosition<100){
          LifterSignal= min(1500,LifterSignal);
       }
       LifterESC.write(constrain(LifterSignal,1400,1600));  
       Serial.print(", current position: ");
       Serial.print(LifterPosition);
       Serial.print(", output: ");
       Serial.println(LifterSignal);  
}

void ScoopLift(){
  int LeftEdgeSensor = analogRead(LeftEdgeSensorPin);
  int RightEdgeSensor = analogRead(RightEdgeSensorPin);
  MoveForward();
  EndTime=millis()+500;
  // while the scoop is down, move it up
    while(analogRead(LifterPotPin)>50 && millis()<EndTime){
      LeftEdgeSensor = analogRead(LeftEdgeSensorPin);
      RightEdgeSensor = analogRead(RightEdgeSensorPin);
      Serial.print(", current position, going up: ");
      Serial.println(analogRead(LifterPotPin));
      LifterESC.write(1600);     
      if((LeftEdgeSensor>550) || (RightEdgeSensor>550) || millis()>EndTime) {
        AllStop();      
      } 
      delay(10);
    }
  LifterESC.write(1500);       
  EndTime=millis()+200;
  while((LeftEdgeSensor<550) && (RightEdgeSensor<550) && millis()<EndTime) {
    LeftEdgeSensor = analogRead(LeftEdgeSensorPin);
    RightEdgeSensor = analogRead(RightEdgeSensorPin);
    MoveForward();  
    delay(10);    
  }
  MoveForward();
  EndTime=millis()+300;  
  // while the scoop is up, move it down
    while(analogRead(LifterPotPin)<900 && millis()<EndTime){
      LeftEdgeSensor = analogRead(LeftEdgeSensorPin);
      RightEdgeSensor = analogRead(RightEdgeSensorPin);
      Serial.print(", current position, going down: ");
      Serial.println(analogRead(LifterPotPin));
      LifterESC.write(1360);
      if((LeftEdgeSensor>550) || (RightEdgeSensor>550) || millis()>EndTime) {
        AllStop();
      }
      delay(10);
    }
    AllStop();
    ScoopSettle();
    LifterESC.write(1500);
    AllStop();
  }
  



void ScoopSettle() {
EndTime=millis()+100;  
float TargetPosition=0;
long oldPosition  = -999;
long oldoldPosition=0;
 
int var;
var=0;
int LiftSpeed;
float gain;
float damping;
int error;
int rate;
gain=0.05;
damping=0.09;
LiftSpeed=90;
while(millis()<EndTime){
  long newPosition =  analogRead(LifterPotPin); ;
  oldoldPosition=oldPosition;
  oldPosition = newPosition;
  TargetPosition=900;
  error=newPosition-TargetPosition;
  rate=oldoldPosition-newPosition;
  LiftSpeed=(int)constrain(gain*(error)+90-damping*rate,70,110);
  if (abs(newPosition-TargetPosition)<50) (int)constrain(0.4*(error)+90-damping*rate,82,98);
  if (abs(newPosition-TargetPosition)<10) LiftSpeed=90;
  LifterESC.write(LiftSpeed);
  delay(10);
  var++;
}
}
