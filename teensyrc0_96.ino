#include <Servo.h>

const int throttleRCPin = 2; // RC input pin 'elevator', used for forward/reverse
//observed neutral ~1500-1530
// observed max: ~1870-1900
//observed min: ~1020-1050
const int steeringRCPin = 3; // RC input pin, 'aileron', used for steering
// observed neutral ~1460-1485
// observed max: ~1880-1910
// observed min: ~1040-1060
const int LifterRCPin = 4; // RC input pin, 'throttle', used for lifter
const int LeftESCPin = 23; // ESC output pin to LHS motors
const int RightESCPin = 20; // ESC output pin to RHS motors
const int LifterESCPin = 17; // ESC output pin to lifter motors
const int LifterPotPin = 15; // Analog input pin to determin lifter position

// Constants for determining ESC parameters
const int ESC_OUTPUT_MAX = 1950;
const int ESC_OUTPUT_MIN = 900;
const int THROTTLE_GAIN = 2.8;

// RC Receiver params
const int PULSEIN_TIMEOUT = 25000;
const int RC_FAILSAFE_MIN = 500;

long LastTriggered;
long EndTime;
long LastAuto = 0;
const int STEERING_NEUTRAL = 1500;
const int THROTTLE_NEUTRAL = 1500;
int steeringSignal = STEERING_NEUTRAL;
int throttleSignal = THROTTLE_NEUTRAL;
int LifterSignal = 1500;
int LifterPosition = 0; // value read from the pot

Servo LeftESC;
Servo RightESC;
Servo LifterESC;

void setup() {
  LeftESC.attach(LeftESCPin);
  RightESC.attach(RightESCPin);
  LifterESC.attach(LifterESCPin);
  Serial.begin(115200);
  LastTriggered = millis();
  pinMode(throttleRCPin, INPUT);
  pinMode(steeringRCPin, INPUT);
  pinMode(LifterRCPin, INPUT);
}

void loop() {
    RCMode();
}

void RCMode() {
  // Serial.print("RCMode v0.8");
  steeringSignal = pulseIn(steeringRCPin, HIGH, PULSEIN_TIMEOUT); // each channel
  throttleSignal = pulseIn(throttleRCPin, HIGH, PULSEIN_TIMEOUT); // Read the pulse width of

  // Failsafe - if signals are too low, set to neutral
  if (throttleSignal < RC_FAILSAFE_MIN || steeringSignal < RC_FAILSAFE_MIN){
    throttleSignal = THROTTLE_NEUTRAL;
    steeringSignal = STEERING_NEUTRAL;
    return;
    } else {
      /*
        apply gain: subtract neutral value, multiply by scaling factor & add neutral
        */
        throttleSignal = int(((throttleSignal - THROTTLE_NEUTRAL) * THROTTLE_GAIN) + THROTTLE_NEUTRAL);
        steeringSignal = int(((steeringSignal - STEERING_NEUTRAL) * THROTTLE_GAIN) + STEERING_NEUTRAL);
    }

  LifterSignal = pulseIn(LifterRCPin, HIGH, PULSEIN_TIMEOUT);
  if (LifterSignal < RC_FAILSAFE_MIN) LifterSignal = 1500;
  // Serial.print(", throttle input: ");
  // Serial.print(throttleSignal);
  // Serial.print(", steering input: ");
  // Serial.print(steeringSignal);
  // Serial.print(", Left output: ");
  // Serial.print(constrain((-throttleSignal + steeringSignal) / 2 + 1500, ESC_OUTPUT_MIN, ESC_OUTPUT_MAX));
  LeftESC.write(constrain((-throttleSignal + steeringSignal) / 2 + 1500, ESC_OUTPUT_MIN, ESC_OUTPUT_MAX));
  // Serial.print(", Right output: ");
  // Serial.print(constrain((throttleSignal + steeringSignal) / 2, ESC_OUTPUT_MIN, ESC_OUTPUT_MAX));
  RightESC.write(constrain((throttleSignal + steeringSignal) / 2, ESC_OUTPUT_MIN, ESC_OUTPUT_MAX));

  // LifterPosition = analogRead(LifterPotPin);
  // Serial.print(", lifter command: ");
  // Serial.print(LifterSignal);
  // if (LifterPosition > 900) {
  // LifterSignal = max(1500, LifterSignal);
  // }
  // if (LifterPosition < 100) {
  //   LifterSignal = min(1500, LifterSignal);
  // }
  LifterESC.write(constrain(LifterSignal, 1200, 1800));
  // Serial.print(", current position: ");
  // Serial.print(LifterPosition);
  // Serial.print(", output: ");
  // Serial.println(LifterSignal);
}
