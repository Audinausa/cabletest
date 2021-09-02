/*
 Cable Tester with Interupt

 The circuit:
 * LED attached from pin 12 to ground
 * pushbutton attached from pin 3 to ground
 
 */

// Constants set based on board/component wiring. These shouldn't change unless we switch boards.
const int BUTTON_PIN = 3; // the pushbutton pin
const int LED_EXT_PIN = 12; // the external LED pin
const int LED_INT = 13; // the LED on the board

const int PS2_PIN1 = 4; // PS2 socket
const int PS2_PIN2 = 5;
const int PS2_PIN3 = 6;
const int PS2_PIN4 = 7;

const int AID_PIN1 = 8; // 4-pin HA socket
const int AID_PIN2 = 9;
const int AID_PIN3 = 10;
const int AID_PIN4 = 11;

const int CABLE_UNKNOWN = 1;
const int CABLE_MISWIRE = 2;
const int CABLE_3X = 3;
const int CABLE_4X = 4;
const int CABLE_5X = 5;

// PS2 (view from front of socket)
//               ___  ___
//(Red)   PIN6  / o [] o \  PIN5 (Yellow)
//(Green) PIN4 ( o  []  o ) PIN3 (Black)
//(White) PIN2  \  o  o  /  PIN1 (Brown)
//               --------

//  __
// (oo)
// (oo)
//  --
//  HA (front of socket)

bool shouldRun = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, LOW);
  
  pinMode(LED_EXT_PIN, OUTPUT);

  pinMode(PS2_PIN1, OUTPUT);
  pinMode(PS2_PIN2, OUTPUT);
  pinMode(PS2_PIN3, OUTPUT);
  pinMode(PS2_PIN4, OUTPUT);

  pinMode(AID_PIN1, INPUT);
  pinMode(AID_PIN2, INPUT);
  pinMode(AID_PIN3, INPUT);
  pinMode(AID_PIN4, INPUT);

  // set initial LED state
  digitalWrite(LED_EXT_PIN, LOW);
  digitalWrite(PS2_PIN1, LOW);
  digitalWrite(PS2_PIN2, LOW);
  digitalWrite(PS2_PIN3, LOW);
  digitalWrite(PS2_PIN4, LOW);
}

void buttonPressed() {
  shouldRun = true;
}

void loop() {
  if (shouldRun) {
    shouldRun = false;
    
    // flash board led to indicate test starting
    flashLed(1, 13);
  
    digitalWrite(LED_EXT_PIN, HIGH);
    delay(2000);
    digitalWrite(LED_EXT_PIN, LOW);

    int cableType = runSmartTest();
    flashLed(cableType, LED_EXT_PIN);

//    int cableType = runTest();
//    flashLed(cableType, LED_EXT_PIN);
  }
}

void flashLed(int x, int ledPin) {
  for (int i = 1; i <= x; i++) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}

int runSmartTest() {

  int PS2Pins[4] = { PS2_PIN1, PS2_PIN2, PS2_PIN3, PS2_PIN4 };
  int HAPins[4] = { AID_PIN1, AID_PIN2, AID_PIN3, AID_PIN4 };

  int InputOutputMap[4] = { 0, 0, 0, 0 };

  for (int i = 0; i < 4; i++) {
    //InputOutputMap[i] = 0;

    digitalWrite(PS2Pins[i], HIGH);
    for (int j = 0; j < 4; j++) {
      if (digitalRead(HAPins[j]) == HIGH) {
        InputOutputMap[i] = j+1;
      }
    }
    digitalWrite(PS2Pins[i], LOW);
  }

  Serial.println("==========================");
  for (int i = 1; i < 5; i++) {
    Serial.println("PS2 Pin " + String(i) + " -> Aid Pin " + String(InputOutputMap[i-1]));
  }
  Serial.println("==========================");

  int cableType = CABLE_UNKNOWN;

  //if (pin1Direct == HIGH && pin2Direct == HIGH)
  if (InputOutputMap[0] == 1 && InputOutputMap[1] == 2)
  {
    // 3 -> 4,  4 -> X
    // if (pin3Direct == LOW && pin3Indirect == HIGH && 
    //     pin4Direct == LOW && pin4Indirect == LOW)
    if (InputOutputMap[2] == 4 && InputOutputMap[3] == 0)
    {
      cableType = CABLE_3X;
    } // 3 -> 4, 4 -> 3
    // else if (pin3Direct == LOW && pin3Indirect == HIGH && 
    //          pin4Direct == LOW && pin4Indirect == HIGH)
    else if (InputOutputMap[2] == 4 && InputOutputMap[3] == 3)
    {
      cableType = CABLE_4X;
    }
    // else if (pin3Direct == HIGH && pin3Indirect == LOW && 
    //          pin4Direct == HIGH && pin4Indirect == LOW)
    else if (InputOutputMap[2] == 3 && InputOutputMap[3] == 4)
    {
      cableType = CABLE_5X;
    }
    else
    {
      cableType = CABLE_UNKNOWN;
    }
  }

  return cableType;
}

int runTest() {
  // test runs too fast to seem like it's doing anything, delay it
  delay(400);
  
  int cableType = CABLE_UNKNOWN;
  
  digitalWrite(PS2_PIN1, HIGH);
  int pin1Direct = digitalRead(AID_PIN1);
  digitalWrite(PS2_PIN1, LOW);
  
  digitalWrite(PS2_PIN2, HIGH);
  int pin2Direct = digitalRead(AID_PIN2);
  digitalWrite(PS2_PIN2, LOW);
  
  digitalWrite(PS2_PIN3, HIGH);
  int pin3Direct = digitalRead(AID_PIN3);
  int pin4Indirect = digitalRead(AID_PIN4);
  digitalWrite(PS2_PIN3, LOW);
  
  digitalWrite(PS2_PIN4, HIGH);
  int pin4Direct = digitalRead(AID_PIN4);
  int pin3Indirect = digitalRead(AID_PIN3);
  digitalWrite(PS2_PIN4, LOW);
  
  // wrap up test
  //digitalWrite(AID_PIN1, LOW);
  //digitalWrite(AID_PIN2, LOW);
  //digitalWrite(AID_PIN3, LOW);
  //digitalWrite(AID_PIN4, LOW);

  if (pin1Direct == HIGH && pin2Direct == HIGH)
  {
    // 3 -> 4,  4 -> X
    if (pin3Direct == LOW && pin3Indirect == HIGH && 
        pin4Direct == LOW && pin4Indirect == LOW)
    {
      cableType = CABLE_3X;
    } // 3 -> 4, 4 -> 3
    else if (pin3Direct == LOW && pin3Indirect == HIGH && 
             pin4Direct == LOW && pin4Indirect == HIGH)
    {
      cableType = CABLE_4X;
    }
    else if (pin3Direct == HIGH && pin3Indirect == LOW && 
             pin4Direct == HIGH && pin4Indirect == LOW)
    {
      cableType = CABLE_5X;
    }
    else
    {
      cableType = CABLE_UNKNOWN;
    }
  }
  //else
  //{
  //  cableType = CABLE_MISWIRE;
  //}
  
//  Serial.println("==========================");
//  Serial.println("Pin 1");
//  Serial.println(pin1Direct);
//  
//  Serial.println("Pin 2");
//  Serial.println(pin2Direct);
//  
//  Serial.println("Pin 3");
//  Serial.println(pin3Direct);
//  
//  Serial.println("Pin 3 Indirect");
//  Serial.println(pin3Indirect);
//  
//  Serial.println("Pin 4");
//  Serial.println(pin4Direct);
//  
//  Serial.println("Pin 4 Indirect");
//  Serial.println(pin4Indirect);
//  
  Serial.println("==========================");
  Serial.println("Cable Type");
  Serial.println(cableType);

  return cableType;
}
