// includes
#include<Wire.h>
#include "math.h"

// constants
const int other_add = 0; //other dev address
const int own_add = 24;

void setup() {
  Serial.begin(115200);
  Wire.begin(own_add);
  Wire.onReceive(receiveEvent); //event handler
  TWAR = (own_add << 1) | 1; // enable broadcasts to be received 
  
}

void loop() {
  // put your main code here, to run repeatedly:

  char c;
  
  if (Serial.available() > 0) {
    c = Serial.read();
    Wire.beginTransmission(other_add); // gets the bus    
    Wire.write(c); // send byte to address on buss
    Wire.endTransmission(); // releases the bus
  }

//  Wire.beginTransmission(other_add); // gets the bus    
//  Wire.write("a"); // send byte to address on buss
//  Wire.endTransmission(); // releases the bus
//  delay(1);
}

void receiveEvent (int howMany) {
  while (Wire.available()> 0) { // check data on BUS
    char c = Wire.read(); //receive byte at I2C BUS 
    Serial.write(c); // echo on terminal
  }
}
